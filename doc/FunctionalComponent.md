# Functional Component
_A device comprises one or more functional components. The following describes the design of a component._

---

## Description

A functional component is defined as a subclass of the `FcmFunctionalComponent` class which in turn is a subclass of the `FcmBaseComponent` class. It implements a specific part of behavioral functionality of the device which is reflected in the _name_ of the component.

In the design, the _behavior_ of the component is typically expressed by a number of scenarios. From these scenarios it can be derived that the component can reside in a specific _current state_.

```cpp
std::string currentState = "NONE";
```

As can be seen, the state is defined as a string. The names of states can be freely chosen, but it is good practice to have a state reflect the component's activity when residing in that state, in a sense that it answers the question as to what the component is currently doing, e.g. "I am &lt;State Name&gt;" (for example "I am Processing"). Doing this makes it much easier to understand the component's behavior.

To implement the concept of an "Any" state and returning to "History", the `historyState` attribute is used.

```cpp
std::string historyState = "NONE";
```

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

For the default constructor the name and optional settings can be supplied.

```cpp
FcmFunctionalComponent(const std::string& nameParam,
                       constFcmSettings& settingsParam = {});
```

For a subclass, this constructor must be called and any specific state-variables can be set in addition.

This will only construct the component, but not initialize the state transition table and choice-points. This will be done by an extra initialization step as described in the next section.

Since a component needs to be able to handle "Timeout" messages on the "Timer" interface, this interface is 'connected' by default.

```cpp
interfaces["Timer"].push_back(this);
```

## Initialization

The construction does not define the states nor the state transition table nor the choice-point evaluation functions. To do this, virtual methods are defined which are to be overridden in the subclass.

```cpp
virtual void setTransitions() = 0;
virtual void setChoicePoints() = 0;
virtual void setStates() = 0;
```

These methods are called in the `initialize()` method.

First the states are set.
```cpp
setStates();
```

In the `setStates()` method the states can be added by csetting the `states` attribute, for example as shown below.

```cpp
states =
{ 
    "Idle",
    "Processing",
    "Done"
};
```

It needs to be checked if the states are not empty. A runtime error will be thrown when the states were not set.

```cpp
if (states.empty())
{
    throw std::runtime_error("No states defined for component \"" + name + "\"!");
}
```

The first state is set to the first state in the list of states.

```cpp
currentState = states[0];
```

Then the choice-points and transitions are set.
```cpp
setChoicePoints();
```

In the `setChoicePoints()` method, all the choice-point evaluation functions must be added by calling the `FCM_ADD_CHOICE_POINT` macro.

The choice-points are also added to the states list to be able to use this list when checking the transition table which is done in the next step.

```cpp
setTransitions();
```

Inside the `setTransitions()` method the transitions can be added by calling the `FCM_ADD_TRANSITION` macro, for example as shown below.

```cpp
FCM_ADD_TRANSITION("Start processing?", Logical, Yes, "Processing",
{
    // ...Action...
});
```

The state transition table is checked to see if it is not empty.
```cpp
if (stateTransitionTable.empty())
{
    throw std::runtime_error("State transition table is empty for component \"" + name + "\"!");
}
```

Note that the overridden initialization methods cannot be called inside the constructor of the base class as that would result in a call to the methods of the base class itself. Hence, the additional initialization step which needs to be part of the constructor of the device.

For subclasses of the `FcmFunctionalComponent` the initialization can be overridden for whatever reason (e.g. constructing specific state variables, starting a timer, sending a first message, etc.). For these functional components the `initialize()` method of the base class can be called.

```cpp
void SomeFunctionalComponent::initialize()
{
    FcmFunctionalComponent::initialize(); // Call base class initialize()

    // ...Additional initialization code for SomeFunctionalComponent...
}
```

## State variables

As was described earlier, some attributes of the component are used inside state transition and evaluation functions. Those specific attributes are called state-variables as these can be technically seen as holding a kind of 'substate' of the component and influence the behavior of the component.

## Add a transition

A transition is uniquely defined by the combination of the state, interface and message. To add this combination to the state transition table and to specify the next state and the action, the component's `addTransition()` method must be called.

```cpp
void addTransition(const std::string& stateName,
                   const std::string& interfaceName,
                   const std::string& messageName,
                   const std::string& nextState,
                   const FcmSttAction& action)
```

The first step is to check if the begin-state is defined. This is done by checking if the state is not a wildcard and if the state is not in the list of states. If this is the case, a runtime error is thrown.

```cpp
if (stateName != "*" &&
    std::find(states.begin(), states.end(), stateName) == states.end() )
{
    throw std::runtime_error("State \"" + stateName + "\" for component \"" + name + "\" does not exist!");
}
```

A similar check is performed for the next state. This state can be the history state, indicated by "H". If the next state is not the history state and the next state is not in the list of states, a runtime error is thrown.
    
```cpp
if (nextState != "H" &&
    std::find(states.begin(), states.end(), nextState) == states.end())
{
    throw std::runtime_error("Next state \"" + nextState + "\" for component \"" + name + "\" does not exist!");
}
```

The next step is to check if there is already a transition with the state. If not, the state is added to the dictionary and set to a newly created empty `FcmSttInterfaces` dictionary.

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
FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Correct server",
{
    timerHandler->cancelTimeout(timerId);
    serverId = message.serverId;
    connectionId = message.connectionId;
});
```

In the example, the `NEXT_STATE` is specified as “Correct server?”. The question mark indicates that the next state is one of the choice-points, which will be described in the next section.

## Add multiple-states transition

On the coding side, a practical way of adding a multiple-states transition is using the FCM_ADD_MULTIPLE_STATES_TRANSITION macro.

```cpp
#define FCM_ADD_MULTIPLE_STATES_TRANSITION(STATES, INTERFACE, MESSAGE, NEXT_STATE, ACTION)  \
    for (const auto& state : std::vector<std::string>STATES)                                \
    {                                                                                       \
        addTransition(state, #INTERFACE, #MESSAGE, NEXT_STATE,                              \
            [this](const std::shared_ptr<FcmMessage>& msg)                                  \
            {                                                                               \
                const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);        \
                ACTION                                                                      \
            });                                                                             \
    }
```

This macro often works with the history-state, which is indicated by “H”. An example of using this macro is given below. Note the use of the parentheses and curly brackets to define the list of states!

```cpp
FCM_ADD_MULTIPLE_STATES_TRANSITION(({ "Connected", "Removing", "Adding"}), Database, KeepAlive, "H",
    databaseInterface->keepAlive();
```

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

It is also added as a 'state' to the list of states to be able to use this list when checking the transition table.

```cpp
states.push_back(choicePointName);
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
To be able to store the reason why a transition was not found, a string is created.

```cpp
std::string notFoundReason;
```

Next, the transition is retrieved by calling the `getTransition()` method.

```cpp
auto transition = getTransition(currentState, interfaceName, messageName, &notFoundReason);
```

If the transition could not be found, there is still the possibility that the wildcard state is used. In this case, the transition is tried to be found again with the wildcard state.

```cpp
if (!transition.has_value())
{
    transition = getTransition("*", interfaceName, messageName);
}
```

If also the wildcard state does not have a transition, a runtime error is thrown with the reason why the transition was not found.

```cpp
if (!transition.has_value())
{
    throw std::runtime_error(notFoundReason);
}
```

When the transition is found, first the next state is set to the next state of the transition. If the next state is the history state, the history state is used instead.

```cpp
std::string nextState = transition->nextState;
if (nextState == "H")
{
    nextState = historyState;
}
```

Doing this makes it possible to use the name of the next state in the transition log.

```cpp
if (logTransitionFunction.has_value())
{
    logTransitionFunction.value()(getLogPrefix("TRANSACTION") +
    "State: \"" + currentState +
    "\" Interface: \"" + interfaceName +
    "\" Message: \"" + messageName +
    "\" Next state: \"" + nextState +
    "\"");
}
```
Finally, the action of the transition is executed and the state is changed.

```cpp
transition->action(message);
currentState = nextState;
```

## Get transition
To retrieve a transition from the state transition table, the `getTransition()` method can be called.

```cpp
std::optional<FcmSttTransition> getTransition(const std::string& stateName,
                                              const std::string& interfaceName,
                                              const std::string& messageName,
                                              std::string* notFoundReason = nullptr) const;
```
The first step is to retrieve the entry in the state transition table for the state.

```cpp
auto state_it = stateTransitionTable.find(stateName);
```

If the state does not exist, the reason is filled in and the method returns.

```cpp
if (state_it == stateTransitionTable.end())
{
    if (notFoundReason != nullptr)
    {
        *notFoundReason = "Transition with begin state \"" + stateName + 
                          "\" for component \"" + name +
                          "\" does not exist in state-transition table!";
    }
    return std::nullopt;
}
```

If the state exists, the interface can be checked and the same procedure is followed.
    
```cpp

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
```
When the interface exists, the message can be checked and the same procedure is followed.

```cpp
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
```
    
Finally, when the transition is found, it can be returned.

```cpp
return message_it->second;
```

## Evaluate choice-point

When the component enters a choice-point, the state machine engine must call the component's `evaluateChoicePoint() `method to evaluate the choice-point.

```cpp
bool evaluateChoicePoint(const std::string& choicePointName) const;
```

Retrieving the choice-point is trivial. The choice-point always exists at this point because the state-transition table was checked.

```cpp
FcmSttEvaluation choicePointEvaluationFunction;
choicePointEvaluationFunction = choicePointTable.at(choicePointName);
```

With the reference to the choice-point retrieved, its evaluation function can be executed and the result be returned.

```cpp
return choicePointEvaluationFunction();
```

## Process message

When a message is received by the component, the `processMessage()` method must be called.

```cpp
void processMessage(const FcmMessage& message)
```

The first step is to store the message in the `lastReceivedMessage` attribute.

```cpp
lastReceivedMessage = std::make_shared<FcmMessage>(message);
```

Next, in order to allow for the concept of using the "Any" state and returning to "History", the `historyState` must be updated.

```cpp
historyState = currentState;
```

Next, the message can be processed by calling the `performTransition()]` method which will execute the state transition and (possibly) change the state of the component.

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
performTransition(choicePointMessage);
```

The loop ends when the new state is not a choice-point.

## Resending message

Consider an example of a state machine for a “Database Manager” which resides in a certain state “Idle” and can receive a message to add something to the database (`Database:AddInd`) or remove something from it (`Database:RemoveId`). These messages contain the parameter `id` that needs to be checked. If it is not correct, the message can be ignored. In case the `id` is correct however, the indication can be forwarded to the database. When the database is ready with the action it will respond with `Database:ReadyInd` which contains the number of elements currently in the database. When something was added, then the state can return to “Idle”, but when something was removed, it must first be checked whether the database is empty.

This behavior can be implemented in the state machine as shown below.

![](https://www.plantuml.com/plantuml/img/TPB1JiCm38RlUGfhHwG-03kCJGEaNT0qJcWW56O34MrI9TxGDl7kk4dAsdLSsjh_vzz_eau3Qy_RgbIV1WcegdaFI67ZJSt6MQN2bx6rzhg2cVVk6uuAuDvPVZG7Wclr8mHh9bXHvNR6VkHoXdWg5bZIEPRh-V9rxhod3KU5G0SOfSzdcnKmU3CQyB8XmlB-ZxoBnscWMytwJGVnZ3s1b5fVlK0nIU5KyuHYMqjZ46aCxVWuTHOMyy3aB507bRBXrC1EXjfR4Fw9mWKavnBzmNKjcXsYfTiTJ4UrF_gtgKXPqvNqJgHntmPDQb7IEvziyewizTnpZixE2xvJiYX_s2y0)

Although the check  of the messages is the same, two choice-points are required because the processing is different. While this technically not a problem, it does make the state machine more complex, especially when there would be even more messages that need to be handled in a similar way (e.g. `Database:ReadInd`, `Database:ChangeInd`, etc.).

For this purpose, FCM introduces two mechanisms:

* `lastReceivedMessage` \
This is an attribute of a Functional Component in which the last received message is stored.
* `resendLastReceivedMessage()` \
This is a method of a Functional Component that resends the last received message as stored in `lastReceivedMessage`.

The `resendLastReceivedMessage()` uses the `resendMessage()` method of the message queue to resend the last received message.

```cpp
messageQueue.resendMessage(lastReceivedMessage);
```

This replaces the message at the top of the message queue, whereby it will be received again by the Functional Component.

Using these mechanisms, the state machine can be simplified as shown below.

![](https://www.plantuml.com/plantuml/img/RPB1JiCm38RlVWfhHwG-03l0fW7IBcY24zeGCes1Y3Qf4c_JXdXtqZPjaubBbFZ__lXtb8KNShAlA_ZIYf7hHawes9TEDwAj0V3VsZJagCQbS_Q8Fu3uO8qywpFZDVm6GaWODywMxBqsdx6mLXN7xHFNzj0gyRpQlAsiSrmAaiVPiDVgTZOHzrsMLhyZeNVol3DTBK00swjNB6xw9j2kM8HZSl4S1zjygTJQg9qP2rqU3ZM0n17yqn31_y8-asExe3nQW0IzD5uqpj1z4-ZXFL8soJJAHSKVqe_LlqR6apeD8_HgqV5ZofW_QbduBD8a_NFvWeqAt-CF)

In this state machine, the “Yes” transition for the “Correct id?” choice-point would be like given below.

```cpp
FCM_ADD_TRANSITION("Correct id?", Logical, Yes, "Processing",
resendMessage(lastReceivedMessage);
);
```

By doing this, the received Database:AddInd or Database:RemoveInd messages will be received again in the “Processing” state and can be handled because at that point it is ensured that the `id` parameter of the message is correct.

Note that the ‘messages’ on the “Logical” interface (i.e. `Logical:Yes` and `Logical:No`) are not stored in the `lastReceivedMessage`.

## Runtime errors

The following runtime errors can occur when using the Functional Component.

| Message | Description |
|---------|-------------|
| State transition table is empty for component "&lt;component_name&gt;"! | During the initialization of the functional component it was determined that the state-transition table is empty. This means that the overridden `setTransitions()` method is not properly implemented. |
| No states defined for component "&lt;component_name&gt;"! | During the initialization of the functional component it was determined that no states are defined. This means that the overridden `setStates()` method is not properly implemented. |
| State "&lt;state&gt;" for component "&lt;component_name&gt;" does not exist! | In the `setTransitions()` method a transition was added with a non-existing state. |
| Next state "&lt;state&gt;" for component "&lt;component_name&gt;" does not exist! | In the `setTransitions()` method a transition was added with a non-existing next-state. |
| Transition "&lt;interface&gt;:&lt;message_type&gt;" on state "&lt;state&gt;" already exists for component "&lt;component_name&gt;!" | In the `setTransitions()` method a transition was added which was already part of the state-transition table. |
| Choice-point "&lt;choice_point_name&gt;" already exists for component "&lt;component_name&gt;"! | In the `setChoicePoints()` method a choice-point was added which already existed. |
| Component "&lt;sender_component_name&gt;" sent the message "&lt;message_type&gt;" to unconnected interface "&lt;interface&gt;!" | The component tries to send a message to an unconnected interface. In most cases this happens when sending to a handler. |
| Interface "&lt;interface&gt;" of component "&lt;component_name&gt;" is already connected to component "&lt;remote_component_name&gt;"! | It is tried to connect an interface that is already connected |
| Component "&lt;component_name&gt;" tries to send message "&lt;message_type&gt;" to interface "&lt;interface&gt;" on index &lt;index&gt; but there are only &lt;number_of_connected_components&gt; components connected! | The component tries to send a message to a multiplied interface on an index that is too large. |
| Last received message cast to invalid message type "&lt;interface&gt;:&lt;message_type&gt;"! | Calling the `FCM_CAST_LAST_RECEIVED_MESSAGE` macro. |
| Messages on interface "&lt;interface&gt;" in state "&lt;stateName&gt;" of component "component_name" are not handled! | In the state-transition table, there are no transitions for the specified interface in the specified state. |
| Message "&lt;message_type&gt;" on interface "&lt;interface&gt;" in state "&lt;state&gt;" of component "&lt;component_name&gt;" is not handled! | In the state-transition table, there is no transition for the specified message in the specified state. |
