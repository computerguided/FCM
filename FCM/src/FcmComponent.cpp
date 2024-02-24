// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// ---------------------------------------------------------------------------------------------------------------------

#include "../inc/FcmComponent.h"
#include "FcmTimerInterface.h"
#include "../inc/FcmLogicalInterface.h"

// ---------------------------------------------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------------------------------------------
FcmComponent::FcmComponent(std::string& nameParam, const std::shared_ptr<FcmMessageQueue>& messageQueueParam) :
    name(nameParam),
    messageQueue(messageQueueParam)
{
    connectInterface("Logical", this);
    connectInterface("Timer", this);

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
        throw std::runtime_error("Choice point already exists: " + choicePointName + " for component " + name);
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

    auto interface = message->interfaceName;
    checkInterface(interface);

    message->receiver = interfaces[interface];
    message->sender = this;

    messageQueue->push(message);
}

// ---------------------------------------------------------------------------------------------------------------------
// Evaluate Choice Point
// ---------------------------------------------------------------------------------------------------------------------
bool FcmComponent::evaluateChoicePoint(const std::string &choicePointName) const
{
    checkChoicePoint(choicePointName);
    return choicePointTable.at(choicePointName)();
}

// ---------------------------------------------------------------------------------------------------------------------
// Process Message
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::processMessage(const FcmMessage& message)
{
    checkMessage(message);

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
        throw std::runtime_error("Messages on interface " + interfaceName + " in state " + currentState + " of component " +
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

    auto action = message_it->second.action;
    auto nextState = message_it->second.nextState;

    // Execute the action.
    action(message);

    currentState = nextState;
}

// ---------------------------------------------------------------------------------------------------------------------
// Connect Interface
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::connectInterface(const std::string &interfaceName, FcmComponent* remoteComponent)
{
    checkInterface(interfaceName);
    interfaces[interfaceName] = remoteComponent;
}

// ---------------------------------------------------------------------------------------------------------------------
// -- Check functions, that throw a runtime error if they fail --
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// Check if the interface exists in the component.
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::checkInterface(const std::string &interfaceName) const
{
    if (interfaces.find(interfaceName) == interfaces.end())
    {
        throw std::runtime_error("Interface " + interfaceName + " for component " + name + " does not exist!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Check if the choice-point exists in the component.
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::checkChoicePoint(const std::string &choicePointName) const
{
    if (choicePointTable.find(choicePointName) == choicePointTable.end())
    {
        throw std::runtime_error("Choice point " + choicePointName + " for component " + name + " does not exist!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Check message.
// ---------------------------------------------------------------------------------------------------------------------
void FcmComponent::checkMessage(const FcmMessage& message) const
{
    // Check if the interface was set and throw a runtime error if it wasn't.
    if (message.interfaceName.empty())
    {
        throw std::runtime_error("Interface of message " + message.name + " is not set!");
    }

    checkInterface(message.interfaceName);
}


