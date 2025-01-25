#include "FcmFunctionalComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmFunctionalComponent::FcmFunctionalComponent(const std::string& nameParam,
                                               const FcmSettings& settingsParam):
    FcmBaseComponent(nameParam,settingsParam)
{
    interfaces["Timer"].push_back(this);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::_initialize()
{
    setStates();
    if (states.empty())
    {
        throw std::runtime_error("No states defined for component \"" + name + "\"!");
    }
    currentState = states[0];

    setChoicePoints();

    setTransitions();
    if (stateTransitionTable.empty())
    {
        throw std::runtime_error("State transition table is empty for component \"" + name + "\"!");
    }

    initialize();
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::addTransition(const std::string& stateName,
                                           const std::string& interfaceName,
                                           const std::string& messageName,
                                           const std::string& nextState,
                                           const FcmSttAction& action)
{
    // Check if the state exists in the state transition table.
    if (stateName != "*" &&
        std::find(states.begin(), states.end(), stateName) == states.end() )
    {
        throw std::runtime_error("State \"" + stateName + "\" for component \"" + name + "\" does not exist!");
    }

    if (nextState != "H" &&
        std::find(states.begin(), states.end(), nextState) == states.end())
    {
        throw std::runtime_error("Next state \"" + nextState + "\" for component \"" + name + "\" does not exist!");
    }

    if (stateTransitionTable.find(stateName) == stateTransitionTable.end())
    {
        stateTransitionTable[stateName] = FcmSttInterfaces();
    }

    if (stateTransitionTable[stateName].find(interfaceName) == stateTransitionTable[stateName].end())
    {
        stateTransitionTable[stateName][interfaceName] = FcmSttMessages();
    }

    if (stateTransitionTable[stateName][interfaceName].find(messageName) != 
        stateTransitionTable[stateName][interfaceName].end())
    {
        throw std::runtime_error( "Transition \"" + interfaceName + ":" + messageName + 
        "\" on state \"" + stateName + "\" already exists for component \"" + name + "\"!");
    }

    stateTransitionTable[stateName][interfaceName][messageName] = FcmSttTransition{action, nextState};
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::addChoicePoint(const std::string& choicePointName,
                                            const FcmSttEvaluation& evaluationFunction)
{
    if (choicePointTable.find(choicePointName) != choicePointTable.end())
    {
        throw std::runtime_error("Choice-point \"" + choicePointName +
            "\" already exists for component \"" + name + "\"!");
    }
    choicePointTable[choicePointName] = evaluationFunction;
    states.push_back(choicePointName);
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmFunctionalComponent::evaluateChoicePoint(const std::string &choicePointName) const
{
    FcmSttEvaluation choicePointEvaluationFunction;
    choicePointEvaluationFunction = choicePointTable.at(choicePointName);
    return choicePointEvaluationFunction();
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmFunctionalComponent::performTransition(const std::shared_ptr<FcmMessage>& message)
{
    auto interfaceName = message->getInterfaceName();
    auto messageName = message->getName();
    std::string notFoundReason;

    // Find the action for the current state, interface and message.
    auto transition = getTransition(currentState, interfaceName, messageName, &notFoundReason);

    if (!transition.has_value())
    {
        // Try to look for a wildcard state.
        transition = getTransition("*", interfaceName, messageName);
    }

    if (!transition.has_value())
    {
        logError(notFoundReason);
        return false;
    }

    std::string nextState = transition->nextState;
    if (nextState == "H")
    {
        nextState = historyState;
    }

    if (logTransitionFunction.has_value())
    {
        logTransitionFunction.value()(getLogPrefix("TRANSACTION") +
            "State: \"" + currentState +
            "\" Interface: \"" + interfaceName +
            "\" Message: \"" + messageName +
            "\" Next state: \"" + nextState +
            "\"");
    }

    transition->action(message);
    currentState = nextState;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::processMessage(const std::shared_ptr<FcmMessage>& message)
{
    lastReceivedMessage = message;
    historyState = currentState;

    if (!performTransition(message))
    {
        return;
    }

    while (choicePointTable.find(currentState) != choicePointTable.end())
    {
        bool result = evaluateChoicePoint(currentState);

        std::shared_ptr<FcmMessage> choicePointMessage;
        if (result)
        {
            choicePointMessage = std::make_shared<Logical::Yes>();
        }
        else
        {
            choicePointMessage = std::make_shared<Logical::No>();
        }
        if (!performTransition(choicePointMessage))
        {
            return;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::resendLastReceivedMessage()
{
    messageQueue.resendMessage(lastReceivedMessage);
}

// ---------------------------------------------------------------------------------------------------------------------
std::optional<FcmSttTransition> FcmFunctionalComponent::getTransition(const std::string& stateName,
                                                                      const std::string& interfaceName,
                                                                      const std::string& messageName,
                                                                            std::string* notFoundReason) const
{
    auto state_it = stateTransitionTable.find(stateName);
    if (state_it == stateTransitionTable.end())
    {
        if (notFoundReason != nullptr)
        {
            *notFoundReason = "Transition with begin state \"" + stateName + "\" for component \"" + name +
                              "\" does not exist in state-transition table!";
        }
        return std::nullopt;
    }

    auto interface_it = state_it->second.find(interfaceName);
    if (interface_it == state_it->second.end())
    {
        if (notFoundReason != nullptr)
        {
            *notFoundReason = "Messages on interface \"" + interfaceName +
                             "\" in state \"" + stateName + "\" of component \"" +
                             name + "\" are not handled!";
        }

        return std::nullopt;
    }

    auto message_it = interface_it->second.find(messageName);
    if (message_it == interface_it->second.end())
    {
        if (notFoundReason != nullptr)
        {
            *notFoundReason = "Message \"" + messageName +
                              "\" on interface \"" + interfaceName + "\" in state \"" +
                              stateName + "\" of component \"" + name + "\" is not handled!";
        }
        return std::nullopt;
    }

    return message_it->second;
}

// ---------------------------------------------------------------------------------------------------------------------
int FcmFunctionalComponent::setTimeout(FcmTime timeout)
{
    return timerHandler.setTimeout(timeout, this);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::cancelTimeout(int timerId)
{
    timerHandler.cancelTimeout(timerId);
}