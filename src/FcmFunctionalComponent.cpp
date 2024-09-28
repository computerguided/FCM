#include "FcmFunctionalComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmFunctionalComponent::FcmFunctionalComponent(const std::string& nameParam,
                                               const std::map<std::string, std::any>& settingsParam):
    FcmBaseComponent(nameParam,settingsParam),
    timerHandler(FcmTimerHandler::getInstance())
{
    interfaces["Timer"].push_back(this);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::initialize()
{
    setTransitions();
    setChoicePoints();

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
void FcmFunctionalComponent::addTransition(const std::string& stateName,
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

    if (stateTransitionTable[stateName][interfaceName].find(messageName) != 
        stateTransitionTable[stateName][interfaceName].end())
    {
        throw std::runtime_error( "Transition already exists: " +
            stateName + " " + interfaceName + " " + messageName + " for component " + name);
    }

    stateTransitionTable[stateName][interfaceName][messageName] = FcmSttTransition{action, nextState};
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmFunctionalComponent::addChoicePoint(const std::string& choicePointName,
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
bool FcmFunctionalComponent::evaluateChoicePoint(const std::string &choicePointName) const
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
void FcmFunctionalComponent::performTransition(const std::shared_ptr<FcmMessage>& message)
{
    auto interfaceName = message->interfaceName;
    auto messageName = message->name;

    auto state_it = stateTransitionTable.find(currentState);
    if (state_it == stateTransitionTable.end())
    {
        throw std::runtime_error("State " + currentState + " for component " + name + " does not exist!");
    }

    auto interface_it = state_it->second.find(interfaceName);
    if (interface_it == state_it->second.end())
    {
        throw std::runtime_error("Messages on interface " + interfaceName +
            " in state " + currentState + " of component " +
            name + " are not handled!");
    }

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
void FcmFunctionalComponent::processMessage(const std::shared_ptr<FcmMessage>& message)
{
    performTransition(message);

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