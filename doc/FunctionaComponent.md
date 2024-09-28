# Functional Component
_A device comprises one or more functional components. The following describes the design of a component._

---

## Description

A functional component is defined as a subclass of the `FcmFunctionalComponent` class which in turn is a subclass of the `FcmBaseComponent` class. It implements a specific part of behavioral functionality of the device which is reflected in the _name_ of the component.

In the design, the _behavior_ of the component is typically expressed by a number of scenarios. From these scenarios it can be derived that the component can reside in a specific _current state_.

```cpp
std::string currentState;
```

As can be seen, the state is defined as a string. The names of states can be freely chosen, but it is good practice to have a state reflect the component's activity when residing in that state, in a sense that it answers the question as to what the component is currently doing, e.g. "I am &lt;State Name&gt;" (for example "I am Processing"). Doing this makes it much easier to understand the component's behavior.

A functional component can also start timers and receive “Timeout” messages. To be able to do this, the reference to the [Timer Handler](TimerHandler.md) is stored.

```cpp
std::shared_ptr<FcmTimerHandler> timerHandler;
```

The _transitions_ between the component's states can occur because of the component receiving a specific message on a specific interface. These transitions are defined in the component's state transition table (STT) which is used when a message is processed.

```cpp
FcmStateTransitionTable stateTransitionTable;
```

Before ending up in a state, a transition can also route to a _choice-point_. This is a special kind of 'state' for which an _evaluation function_ is executed when the state is entered.

The evaluation function of a choice-point does not take any arguments and the evaluation is done based on one or more of the attributes of the component.

The evaluation of a choice-point results in a logical value: true or false. Based on this result, the corresponding message "Yes" or "No" on the "Logical" interface is created and immediately processed. Note that each component has an implicit "Logical" interface by default.

By definition a choice-point has two transitions associated with the "Yes" and "No" message. These transitions in turn can also route to another choice-point if so required.

Inside the component, the list of choice-points is explicitly defined in the component's _choice-point table_.

```cpp
FcmChoicePointTable choicePointTable;
```

Although the name of a choice-point can be freely chosen, it would be good practice to formulate the name as a closed question that can be answered with "Yes" or "No" and as such have it end with a question mark (for example "Max clients reached?").

## Construction

For the default constructor the name, message-queue and timer-handler of the component are set. Optionally, settings can be supplied.

```cpp
FcmFunctionalComponent(const std::string& nameParam,
                        const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                        const std::shared_ptr<FcmTimerHandler>& timerHandlerParam,
                        const std::map<std::string, std::any>& settingsParam = {});
```

For a subclass, this constructor must be called and any specific state-variables can be set in addition.

This will only construct the component, but not initialize the state transition table and choice-points. This will be done by an extra initialization step as described in the next section.

Since a component needs to be able to handle "Timeout" messages on the "Timer" interface, this interface is 'connected' by default.

```cpp
interfaces["Timer"].push_back(this);
```

## Initialization

The construction does not define the state transition table nor the choice-point evaluation functions. To do this, two virtual methods are defined which are to be overridden in the subclass.

```cpp
virtual void setTransitions() = 0;
virtual void setChoicePoints() = 0;
```

These two methods are called in the `initialize()` method.

```cpp
setTransitions();
setChoicePoints();
```

For an actual component which is defined as a subclass, these two methods must be overridden.

Inside the `setTransitions()` method the transitions can be added by calling the `FCM_ADD_TRANSITION` macro.

Likewise, in the `setChoicePoints()` method, all the choice-point evaluation functions must be added by calling the `FCM_ADD_CHOICE_POINT` macro.

As a final step inside the `initialize()` method, the first state of the component is set to that of the first transition. A runtime error will be thrown when the state transition table was not set.

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

Note that the overridden initialization methods cannot be called inside the constructor of the base class as that would result in a call to the methods of the base class itself. Hence, the additional initialization step which needs to be part of the constructor of the device.

## State variables

As was described earlier, some attributes of the component are used inside state transition and evaluation functions. Those specific attributes are called state-variables as these can be technically seen as holding a kind of 'sub-state' of the component and influence the behavior of the component.

## Add a transition

A transition is uniquely defined by the combination of the state, interface and message. To add this combination to the state transition table and to specify the next state and the action, the component's `addTransition()` method must be called.

```cpp
void addTransition(const std::string& stateName,
                   const std::string& interfaceName,
                   const std::string& messageName,
                   const std::string& nextState,
                   const FcmSttAction& action)
```

The first step performed in this method is to check if the state exists. If the state does not exist, the state is added to the dictionary and set to a newly created empty `FcmSttInterfaces` dictionary.

```cpp
if (stateTransitionTable.find(stateName) == stateTransitionTable.end())
{
    stateTransitionTable[stateName] = FcmSttInterfaces();
}
```

Next it must be checked if the interface exists for the state. If this is not the case, the interface is added to the dictionary and set to a newly created empty `FcmSttMessages` dictionary.

```cpp
if (stateTransitionTable[stateName].find(interfaceName) == stateTransitionTable[state].end())
{
    stateTransitionTable[stateName][interfaceName] = FcmSttMessages();
}
```

Before adding the new message, throw a run-time error if the message already exists

```cpp
if (stateTransitionTable[stateName][interfaceName].find(messageName) != 
    stateTransitionTable[stateName][interfaceName].end())
{
    throw std::runtime_error( "Transition already exists: " + 
                              stateName + " " + interfaceName + " " + messageName +
                              " for component " + name);
}
```

When the message does not exist yet, add the message with the new `nextState` and `action`.

```cpp
stateTransitionTable[stateName][interfaceName][messageName] = FcmSttTransition{action, nextState};
```

The `addTransition()` method is not called directly, instead the `FCM_ADD_TRANSITION` macro is used. The macro takes five arguments: the current state, the interface, the message, the next state and the action. The action is a lambda function that is executed when the state transition occurs. Inside the action, the message can be used to extract data and to send messages to other components or handlers. This is possible because the message is automatically cast to the correct type.

```cpp
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)       \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                      \
    [this](const std::shared_ptr<FcmMessage>& msg)                              \
    {                                                                           \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
        ACTION                                                                  \
    })
```

By specifying the `ACTION` as the last argument, the macro can be conveniently used. Consider for example a situation of a regularly advertising client that is waiting for a connection request from a server:

```cpp
FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Corect server",
{
    timerHandler->cancelTimeout(timerId);
    serverId = message.serverId;
    connectionId = message.connectionId;
});
```

In the example, the `NEXT_STATE` is specified as “Correct server?”. The question mark indicates that the next state is one of the choice-points, which will be described in the next section.

## Add a choice-point

To add a choice-point to the table, the component's `addChoicePoint()` method can be called.

```cpp
void addChoicePoint( const std::string& choicePointName,
                     const FcmSttEvaluation& evaluationFunction)
```

Before adding the new choice-point, check if it already exists. If the choice point exists, a runtime error is thrown, printing the choice-point name.

```cpp
if (choicePointTable.find(choicePointName) != choicePointTable.end())
{
    throw std::runtime_error("Choice-point " + choicePointName +
        " already exists " + " for component " + name);
}
```

When the choice-point does not exist yet, the choice-point can be added.

```cpp
choicePointTable[choicePointName] = evaluationFunction;
```

The `addChoicePoint()` method is not called directly, but instead the `FCM_ADD_CHOICE_POINT` macro is used which takes two arguments: the choice-point (i.e. name) and the evaluation. The evaluation is a lambda function that is executed when the state is entered. Inside the evaluation, the state variables can be used to make a decision.

```cpp
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT,                        \
    [this]()                                            \
    {                                                   \
        EVALUATION                                      \
    })
```

Consider the example where a choice-point is defined to check whether a server is whitelisted.

```cpp
FCM_ADD_CHOICE_POINT( "Correct Server?",
{
    // Determine whether the server is in the whitelist.
    auto it = std::find(serverWhitelist.begin(),
                        serverWhitelist.end(), serverId);

    return it != serverWhitelist.end();
});
```

## Perform transition

For a component to perform a transition, its `performTransition()` method must be called.

```cpp
void performTransition(const FcmMessage& message)
```

The first step is to get the names of the interface and the message, which were set during creation of the message.

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

When the interface exists, finally it is checked if the message exists for the found interface and a runtime error is thrown if it doesn't.

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

When the component enters a choice-point, the state machine engine must call the component's `evaluateChoicePoint() `method to evaluate the choice-point.

```cpp
bool evaluateChoicePoint(const std::string& choicePointName) const;
```

Since choice-points must be added explicitly, retrieving the choice-point is done by catching the exception when it was not added.

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

If the evaluation function was found, it can be executed and the result be returned.

```cpp
return choicePointEvaluationFunction();
```

## Process message

When a message is received by the component, the `processMessage()` method must be called.

```cpp
void processMessage(const FcmMessage& message)
```

The first step is to process the message by calling the <code>[performTransition()](#heading=h.pel4tooggxn2)</code> method which will execute the state transition and (possibly) change the state of the component.

```cpp
performTransition(message);
```

Next, the component must check if the new state is a choice-point, i.e. whether it can be found in `choicePointTable`. This is done in a while-loop to allow for consecutive choice-points where the transition from a choice-point ends-up in another choice-point.

```cpp
while (choicePointTable.find(currentState) != choicePointTable.end())
{
    // ...
}
```

If the new state is a choice-point, the evaluation function of the choice-point must first be executed.

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

