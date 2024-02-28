// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------------------------------------------
FcmComponent::FcmComponent(std::string& nameParam,
                           const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                           const std::shared_ptr<FcmTimerHandler>& timerHandlerParam)
    : name(nameParam), messageQueue(messageQueueParam), timerHandler(timerHandlerParam)
{
    interfaces["Timer"] = this;
}

// ---------------------------------------------------------------------------------------------------------------------
// Initialize
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
// Add Transition
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
// Add Choice Point
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
// Determine whether the current state is a choice-point.
// ---------------------------------------------------------------------------------------------------------------------
bool FcmComponent::atChoicePoint() const
{
    return choicePointTable.find(currentState) != choicePointTable.end();
}

// ---------------------------------------------------------------------------------------------------------------------
// Send Message
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::sendMessage(const std::shared_ptr<FcmMessage>& message)
{
    message->timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
    try
    {
        message->receiver = interfaces.at(message->interfaceName);
    }
    catch (const std::out_of_range& e)
    {
        message->receiver = nullptr;
    }

    messageQueue->push_back(message);
}

// ---------------------------------------------------------------------------------------------------------------------
// Evaluate Choice Point
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
// Process Message
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::processMessage(const FcmMessage& message)
{
    auto interfaceName = message.interfaceName;
    auto messageName = message.name;

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
// Connect Interface
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::connectInterface(const std::string &interfaceName, FcmComponent* remoteComponent)
{
    interfaces[interfaceName] = remoteComponent;
}


