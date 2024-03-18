// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// Documentation: https://github.com/computerguided/FCM/blob/main/FCM/doc/Component.md
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmComponent::FcmComponent(const std::string& nameParam,
                           const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                           const std::shared_ptr<FcmTimerHandler>& timerHandlerParam,
                           const std::map<std::string, std::any>& settingsParam):
                           FcmBaseComponent(nameParam,messageQueueParam,settingsParam),
                           timerHandler(timerHandlerParam)
{
    interfaces["Timer"] = this;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::initialize()
{
    setTransitions();
    setChoicePoints();

    // Find the first state in the state transition table.
    if (!stateTransitionTable.empty())
    {
        currentState = stateTransitionTable.begin()->first;
    }
    else
    {
        throw std::runtime_error("State transition table is empty for component " + name);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::addTransition(const std::string& stateName,
                                 const std::string& interfaceName,
                                 const std::string& messageName,
                                 const std::string& nextState,
                                 const FcmSttAction& action)
{
    if (stateTransitionTable.find(stateName) == stateTransitionTable.end())
    {
        stateTransitionTable[stateName] = FcmSttInterfaces();
    }

    if (stateTransitionTable[stateName].find(interfaceName) == stateTransitionTable[stateName].end())
    {
        stateTransitionTable[stateName][interfaceName] = FcmSttMessages();
    }

    if (stateTransitionTable[stateName][interfaceName].find(messageName) != stateTransitionTable[stateName][interfaceName].end())
    {
        throw std::runtime_error( "Transition already exists: " +
            stateName + " " + interfaceName + " " + messageName + " for component " + name);
    }

    stateTransitionTable[stateName][interfaceName][messageName] = FcmSttTransition{action, nextState};
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::addChoicePoint(const std::string& choicePointName,
                                  const FcmSttEvaluation& evaluationFunction)
{
    if (choicePointTable.find(choicePointName) != choicePointTable.end())
    {
        throw std::runtime_error("Choice-point " + choicePointName +
            " already exists " + " for component " + name);
    }
    choicePointTable[choicePointName] = evaluationFunction;
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmComponent::evaluateChoicePoint(const std::string &choicePointName) const
{
    FcmSttEvaluation choicePointEvaluationFunction;

    try
    {
        choicePointEvaluationFunction = choicePointTable.at(choicePointName);
    }
    catch (const std::out_of_range& e)
    {
        throw std::runtime_error("Choice point " + choicePointName + " for component " + name + " does not exist!");
    }

    return choicePointEvaluationFunction();
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::performTransition(const std::shared_ptr<FcmMessage>& message)
{
    auto interfaceName = message->interfaceName;
    auto messageName = message->name;

    // Check if the current state exists in the state transition table and throw a runtime error if it doesn't.
    auto state_it = stateTransitionTable.find(currentState);
    if (state_it == stateTransitionTable.end())
    {
        throw std::runtime_error("State " + currentState + " for component " + name + " does not exist!");
    }

    // Check if the interface exists in the state transition table and throw a runtime error if it doesn't.
    auto interface_it = state_it->second.find(interfaceName);
    if (interface_it == state_it->second.end())
    {
        throw std::runtime_error("Messages on interface " + interfaceName +
            " in state " + currentState + " of component " +
            name + " are not handled!");
    }

    // Check if the message exists in the state transition table and throw a runtime error if it doesn't.
    auto message_it = interface_it->second.find(messageName);
    if (message_it == interface_it->second.end())
    {
        throw std::runtime_error("Message " + messageName +
            " on interface " + interfaceName + " in state " +
                currentState + " of component " + name + " is not handled!");
    }

    message_it->second.action(message);
    currentState = message_it->second.nextState;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::processMessage(const std::shared_ptr<FcmMessage>& message)
{
    performTransition(message);

    // Loop until the new state of the receiver is not a choice-point.
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
        performTransition(choicePointMessage);
    }
}