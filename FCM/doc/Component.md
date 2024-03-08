# FCM Component
***
_A device comprises one or more functional components. The concept of such a component is described in this document._
***

## Table of contents

1. [Description](#description)
2. [Construction](#construction)
3. [Initialization](#initialization)
4. [Interfacing](#interfacing)
5. [State-variables](#state-variables)
6. [Settings](#settings)
7. [Adding a transition](#adding-a-transition)
8. [Adding a choice-point](#adding-a-choice-point)
9. [Prepare a message](#prepare-a-message)
10. [Send a message](#send-a-message)
11. [Perform transition](#perform-transition)
12. [Evaluate choice-point](#evaluate-choice-point)
13. [Connect interface](#connect-interface)
14. [Process message](#process-message)

## Description
A _component_ is defined as a subclass of the [``FcmComponent``](../inc/FcmComponent.h) class. It implements a specific part of behavioral functionality of the device which is reflected in the __name__ of the component which is set at the initialization of the component.

```cpp
const std::string name;
```

Preferably the name is unique within the device but can be freely chosen. However, it is good practice to have the name reflect the component's functionality in a sense that it answers the question as to what the component does, e.g. "I am a &lt;Component Name&gt;". Doing this makes it much easier to understand the device architecture, ensuring a modular design and preventing errors, mistakes and inefficient architectures.

The name is used to identify and display the component in the component diagram. As a guideline, when displaying the component in the documentation, the name of the component is printed in the diagram with each word capitalized and separated by a space or newline (e.g. "Connection Handler").

![Component](https://www.plantuml.com/plantuml/png/JOt1YiCm30Nl_WgHByaI5lQoIQ4vzYBirzHKbesiGFFtfHHAhyCCCtPdyxr8kEK4Gcb-K4AFbfldgc_S7ORgHqFezwoDZGl5MbfLg9_z490XEkictqh2bNVn5G__aRnfWJf5gCF29wTf-Jyjy--y4dSQfggA-7fVz8yr2Mm88pJjHPu0)

For the corresponding class name in the code, the spaces are omitted (e.g. ``ConnectionHandler``).

The _behavior_ of the component is expressed by a number of scenarios. From these scenarios it can be derived that the component can reside in a specific _current state_.

```cpp
std::string currentState;
```

As can be seen, the state is defined as a string, i.e. the _state-name_ which can be freely chosen. However, it is good practice to have the state reflect the component's current activity in a sense that it answers the question as to what the component is currently doing, e.g. "I am &lt;State Name&gt;" (for example "I am Processing").

Doing this makes it much easier to understand the component's behavior.

The _transitions_ between the component's states can occur because of the component receiving a specific _message_ on a specific _interface_ (discussed in more detail in "[Component interfacing](#interfacing)"). These transitions are defined in the component's [_state-transition table_](StateTransitionTable.md) (STT) which is used when a message is processed (see "[Process message](#process-message)").

```cpp
FcmStateTransitionTable stateTransitionTable;
```

Before ending up in a state, a transition can also route to a _choice-point_. This is a special kind of 'state' for which an _evaluation function_ is executed when the state is entered.

The evaluation function of a choice-point does not take any arguments and the evaluation is done based on one or more of the attributes of the component.

The evaluation of a choice-point results in a logical value: ``true`` or ``false``. Based on this result, the corresponding message "Yes" or "No" on the "Logical" interface is created and immediately processed. Note that each component has an implicit "Logical" interface by default.

By definition a choice-point has two transitions associated with the "Yes" and "No" message and that these transitions in turn can also route to another choice-point if so required.

Inside the component, the list of choice-points is explicitly defined in the component's _choice-point table_.

```cpp
FcmChoicePointTable choicePointTable;
```

Although the name of a choice-point can be freely chosen, it would be good practice to formulate the name as a closed question that can be answered with "Yes" or "No" and as such have it end with a question mark (for example "Max clients reached?").

Inside a transition, a component send messages to other components(see "[Send a message](#send-a-message)"). To be able to do this, the component has a reference to the [_message-queue_](../inc/FcmMessageQueue.h) that it is supposed to use.

```cpp
const std::shared_ptr<FcmMessageQueue> messageQueue;
```

## Construction

For the default constructor the name, message-queue and timer-handler (see "[Timer Handler](TimerHandler.md)") of the component are set.

```cpp
FcmComponent(std::string& nameParam,
             const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
             const std::shared_ptr<FcmTimerHandler>& timerHandlerParam)
```

For a subclass, this constructor must be called and any specific state-variables can be set in addition as shown in the example below.

```cpp
Connector(std::string& name,
          const std::shared_ptr<FcmMessageQueue>& messageQueue,
          const std::shared_ptr<FcmTimerHandler>& timerHandlerParam,
          const std::shared_ptr<Settings>& settingsParam):
    FcmComponent(name, messageQueue, timerHandlerParam),
    settings(settingsParam),
    clientId(settings->clientId),
    serverId(0),
    connectionId(0){};
```

This will only construct the component, but not _initialize_ the state transition table and choice-points. This will be done by an extra initialization step as described in the next section.

Since a component needs to be able to handle "Timeout" messages on the "Timer" interface, this interface is 'connected' by default.

```cpp
interfaces["Timer] = this;
```

## Initialization
The construction does not define the state transition table nor the choice-point evaluation functions. To do this, two virtual methods are defined which are to be overridden in the subclass:

```cpp
virtual void setTransitions() = 0;
virtual void setChoicePoints() = 0;
```

These two methods are called in the ``initialize()`` method.

```cpp
setTransitions();
setChoicePoints();
```

For an actual component which is defined as a subclass of the ``FcmComponent``, these two methods must be overridden.

Inside the ``setTransitions()`` method the transitions can be added by calling the ``FCM_ADD_TRANSITION`` macro (see "[Adding a transition](#adding-a-transition)").

Likewise, in the ``setChoicePoints()`` method, all the choice-point evaluation functions must be added by calling the ``FCM_ADD_CHOICE_POINT`` macro (see "[Adding a choice-point](#adding-a-choice-point)").

As a final step inside the ``initialize()`` method, the first state of the component is set to that of the first transition. A runtime error will be thrown when the state transition table was not set.

```cpp
if (!stateTransitionTable.empty())
{
    currentState = stateTransitionTable.begin()->first;
}
else
{
    throw std::runtime_error("State transition table is empty for component " + name);
}
```

Note that the overridden initialization methods cannot be called inside the constructor of the base class as that would result in a call to the methods of the base class itself. Hence, the additional initialization step.

## Interfacing
A component can have one or more _interfaces_, depending on the design of the component.

It is good engineering practice to define the interfaces beforehand as much as possible. This means; defining the messages and their parameters (see "[Messages](Messages.md)"). Doing this, allows for independently further defining the state-machine of the component.

The interface completely shields the component from the environment in which it is running. A more detailed description is given in "[Interfaces](Interfaces.md)".

Note that grouping the messages into interfaces is technically not really necessary; some state-machine frameworks don't even specify interfaces, and for the method presented here, nothing prevents the developer from using only one interface containing all the messages. However, using interfaces has the following benefits:
* It creates a more modular design.
* Message names can be reused in different interfaces.
* Logging becomes easier when the interface is specified.

However, one of the most important advantages of using interfaces is that the state-machine engine can be more efficient.

All the interfaces a component uses are defined in its dictionary ``interfaces`` attribute.

```cpp
std::map<std::string, const FcmComponent*> interfaces;
```

As can be seen from the definition, an interface holds a reference to ```FcmComponent``` which is set when the interface is connected to the component (see "[Connect interface](#connect-interface)").

To be able to use an interface (e.g. send the messages defined in the interface), the header file of the interface needs to be included. For example, by default the ``FcmComponent`` adds the "Logical" interface by including the ``FcmLogicalInterface.h``.

## State-variables
As was described earlier, some attributes of the component are used inside state transition and evaluation functions. Those specific attributes are called _state-variables_ as these can be technically seen as holding a kind of 'sub-state' of the component and influence the behavior of the component.

## Settings
Apart from state-variables, a component also has _settings_. This is a dictionary that is offered to the component when it is instantiated, but cannot be changed by the component itself. This is implemented by storing the settings as a constant attribute.

```cpp
const std::map<std::string, std::any> settings;
```

The settings are used to configure the component and can be used to set the initial state-variables. The settings are set when the component is constructed and are used in the overridden constructor of the subclass.

As an example, suppose a component has a setting ``clientId``, then the settings can be used as follows in which also a runtime error is thrown when the setting is incorrectly casted.

```cpp
try
{
    clientId = std::any_cast<uint32_t>(settings.at("clientId"));
}
catch (const std::bad_any_cast& e)
{
    throw std::runtime_error("Connector: " + name + " settings error: " + e.what());
};
```

## Adding a transition
As specified in "[State transition table](StateTransitionTable.md)", a transition is uniquely defined by the combination of the state, interface and message. To add this combination to the state transition table and to specify the next state and the action, the component's ``addTransition()`` method must be called.

```cpp
void addTransition(const std::string& stateName,
                   const std::string& interfaceName,
                   const std::string& messageName,
                   const std::string& nextState,
                   const FcmSttAction& action)
```

The first step is to check if the state exists. If the state does not exist, the state is added to the dictionary and set to a newly created empty ``FcmSttInterfaces`` dictionary.

```cpp
if (stateTransitionTable.find(stateName) == stateTransitionTable.end())
{
    stateTransitionTable[stateName] = FcmSttInterfaces();
}
```

Next it must be checked if the interface exists for the state. If this is not the case, the interface is added to the dictionary and set to a newly created empty ``FcmSttMessages`` dictionary.

```cpp
if (stateTransitionTable[stateName].find(interfaceName) == stateTransitionTable[state].end())
{
    stateTransitionTable[stateName][interfaceName] = FcmSttMessages();
}
```

Before adding the new message, throw a run-time error if the message already exists

```cpp
if (stateTransitionTable[stateName][interfaceName].find(messageName) != stateTransitionTable[stateName][interfaceName].end())
{
    throw std::runtime_error( "Transition already exists: " + 
                              stateName + " " + interfaceName + " " + messageName +
                              " for component " + name);
}
```

When the message does not exist yet, add the message with the new ``nextState`` and ``action``.

```cpp
stateTransitionTable[stateName][interfaceName][messageName] = FcmSttTransition{action, nextState};
```

Using the ``addTransition()`` method of the component, a transition could be added as shown in the following example.

```cpp
addTransition("Processing", "ConnectionInterface", "ClientConnected", "Processing", 
    [this](const FcmMessage& msg)
    {
        const auto& message = static_cast<ConnectionInterface::ClientConnected&>(msg);
        std::cout << "Client connected: " << message.clientData << std::endl;
        numClients++;
    });
```

Looking at the example, the amount of code can be simplified by using the ``FCM_ADD_TRANSITION`` macro as shown below.

```cpp
#define ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)   \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,              \
    [this](const FcmMessage& msg)                                       \
    {                                                                   \
        const auto& message = dynamic_cast<INTERFACE::MESSAGE&>(msg);    \
        ACTION                                                          \
    })
```

Adding the transition in the example can now be done as shown below.

```cpp
FCM_ADD_TRANSITION("Processing", ConnectionEvents, ClientConnected, "Processing",
{
    std::cout << "Client connected: " << message.clientData << std::endl;
    numClients++;
});
```

## Adding a choice-point
To add a choice-point to the table, the component's ``addChoicePoint()`` method can be called.

```cpp
void addChoicePoint( const std::string& choicePointName,
                     const FcmSttEvaluation& evaluationFunction)
```

Before adding the new choice-point, check if it already exists. If the choice point exists, throw a runtime error, printing the choice-point name.

```cpp
if (choicePointTable.find(choicePointName) != choicePointTable.end())
{
    throw std::runtime_error("Choice-point " + choicePointName +
        " already exists " + " for component " + name);
}
```

When the choice-point does not exist yet, the choice-point with the new evaluation function can be added.

```cpp
choicePointTable[choicePointName] = evaluationFunction;
```

Using the ``addChoicePoint()`` method, a choice-point could be added as shown in the following example.

```cpp
addChoicePoint("Max clients?",[this]()
{
    return (numClients >= settings.maxClients);
});
```

Looking at the example, the call can be simplified by using the FCM_ADD_CHOICE_POINT macro as shown below.

```cpp
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT, [this]                 \
    {                                                   \
        EVALUATION                                      \
    });
```

Adding the choice-point in the example can now be done as shown below.

```cpp
FCM_ADD_CHOICE_POINT("Max clients?",
{
    return (numClients >= maxClients);
});
```

## Prepare a message

When performing a transition, a component can send a message by performing the following steps:
1. Create the message by instantiating the proper class as defined in the corresponding interface file.
1. Set the message parameters.
1. Call the ``sendMessage()`` method (see "[Send a message](#send-a-message)") with the created message as the single argument.

To help with the first step, a macro is defined:

```cpp
#define FCM_PREPARE_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE) \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>()
```

For example, preparing the "ConnectAck" message on the "Transceiving" interface can then be done as follows:

```cpp
FCM_PREPARE_MESSAGE(connectAck, Transceiving, ConnectAck);
```

Suppose that in this example, the message has one parameter ``connectionId`` that must be set to the ``connectionId`` state variable of the component. This can then be done as follows:

```cpp
connectAck->connectionId = connectionId;
```

When all message parameters are set, the message can be sent, which can be done using the ``FCM_SEND_MESSAGE()`` macro.

```cpp 
FCM_SEND_MESSAGE(connectAck);
```

This macro is defined as:

```cpp
#define FCM_SEND_MESSAGE( MESSAGE ) \
    sendMessage(MESSAGE)
```

The ``sendMessage()`` method (discussed in the next section) can of course also be called directly, but using the macro gives more symmetrical code with the ``FCM_PREPARE_MESSAGE()``.


## Send a message
When performing a transition, it must be possible for a component to 'send' a message. This is done by calling its ``sendMessage()`` property.

```cpp
void sendMessage(const std::shared_ptr<FcmMessage>& message)
```

When constructing the message, the ``interfaceName`` property was already set (see "[Messages](Messages.md)") and can now be used to retrieve a connected receiver, but only if the interface was connected.

```cpp
try
{
    message->receiver = interfaces.at(message->interfaceName);
}
catch (const std::out_of_range& e)
{
    message->receiver = nullptr;
}
```

Note that to allow testing with unconnected interfaces, when the interface was not connected (see "[Connect interface](#connect-interface)"), the ``receiver`` will be the ``nullptr`` which will be handled further in message processing loop of the Device (see ["Device - Processing message"](Device.md#process-messages)).

With the ``receiver`` set, the message can be added to the message queue (see "[Message Queue](MessageQueue.md)") that was supplied when constructing the component (see "[Component construction](#construction)").

```cpp
messageQueue->push(message);
```

## Perform transition

To perform a transition, the component's ``performTransition()`` method must be called.

```cpp
void performTransition(const FcmMessage& message)
```

The first step is to get the names of the interface and the message, which were set during creation of the message (see "[Messages](Messages.md)").

```cpp
auto interfaceName = message->interfaceName;
auto messageName = message->name;
```

Next it must be checked if the current state exists in the state transition table and a runtime error must be thrown if it doesn't.

```cpp
auto state_it = stateTransitionTable.find(currentState);
if (state_it == stateTransitionTable.end())
{
    throw std::runtime_error("State " + currentState + " for component " + name + " does not exist!");
}
```

The same must be done for the interface.

```cpp
auto interface_it = state_it->second.find(interfaceName);
if (interface_it == state_it->second.end())
{
    throw std::runtime_error("Messages on interface " + interfaceName + " in state " + currentState + " of component " +
        name + " are not handled!");
}
```

When the interface exists, finally it is checked if the message exists in the state transition table and a runtime error is thrown if it doesn't.

```cpp
auto message_it = interface_it->second.find(messageName);
if (message_it == interface_it->second.end())
{
    throw std::runtime_error("Message " + messageName +
        " on interface " + interfaceName + " in state " +
            currentState + " of component " + name + " is not handled!");
}
```

When the proper transition is found, the action can be executed and the state changed.

```cpp
message_it->second.action(message);
currentState = message_it->second.nextState;
```

## Evaluate choice-point

When the component entered a choice-point, the state machine engine must call the component's method to evaluate the choice-point.

```cpp
bool evaluateChoicePoint(const std::string& choicePointName) const;
```

Since choice-points must be added explicitly (see "[Adding a choice-point](#adding-a-choice-point)") retrieving the choice-point is done by catching the exception when it was not added.

```cpp
FcmSttEvaluation choicePointEvaluationFunction;

try
{
    choicePointEvaluationFunction = choicePointTable.at(choicePointName);
}
catch (const std::out_of_range& e)
{
    throw std::runtime_error("Choice point " + choicePointName + 
    " for component " + name + " does not exist!");
}
```

If the evaluation function was found, it can be executed and the result be returned to the calling state machine engine.

```cpp
return choicePointEvaluationFunction();
```

## Connect interface

To connect and interface to a remote receiver, the ``connectInterface()`` method must be called for the component.

```cpp
void connectInterface(const std::string& interfaceName, FcmComponent* remoteComponent);
```

For the component itself, connecting means nothing more than setting the corresponding entry in the dictionary.

```cpp
interfaces[interfaceName] = remoteComponent;
```

This method is called when the component structure is set-up during the construction of the device (see "[Device](Device.md)"). In there, the ``connectInterface()`` for the components at both ends must be called. To simplify this, the ``FCM_CONNECT_INTERFACE`` is defined.

```cpp
#define FCM_CONNECT_INTERFACE( INTERFACE, COMPONENT_1, COMPONENT_2 ) \
    COMPONENT_1->connectInterface(#INTERFACE, COMPONENT_2)           \
    COMPONENT_2->connectInterface(#INTERFACE, COMPONENT_1)
```

## Process message

When a message is received by the component, the component's ``processMessage()`` method must be called.

```cpp
void executeStateMachine(const FcmMessage& message)
```

The first step is to process the message by calling the [``performTransition()``](#perform-transition) method which will execute the state transition and (possibly) change the state of the component.

```cpp
performTransition(message);
```

Next, the component must check if the new state is a choice-point.

```cpp
while (choicePointTable.find(currentState) != choicePointTable.end())
{
    ...
}
```

If the new state is a choice-point, the evaluation function of the choice-point must be executed.

```cpp
bool result = evaluateChoicePoint(currentState);
```

Depending on the result of the evaluation, the corresponding message "Yes" or "No" on the "Logical" interface must be created.

```cpp
std::shared_ptr<FcmMessage> choicePointMessage;
if (result)
{
    choicePointMessage = std::make_shared<Logical::Yes>();
}
else
{
    choicePointMessage = std::make_shared<Logical::No>();
}
```

The message can be processed immediately, after which the loop will continue until the new state is not a choice-point.

```cpp
performTransition(*choicePointMessage);
```