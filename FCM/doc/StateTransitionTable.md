# State Transition Table
_The state transition table is a data structure that is used to define the behavior of a component. It is a table that contains the states and events of the component and the actions that are executed when a state transition occurs._
***

## Description

A _state machine_ defines the behavior of a component by defining a structured deterministic behavioral model in which a component resides in a specific _state_ and can only exit this state and move to another state due to a _message_. Moving from one state to another state is called a _transition_. It is also possible to return to the original state. This is a self-transition.

A state machine can be defined by a _state transition table_. In the FCM described here, this table comprise the following columns:
* _State_: the current state of the component.
* _Interface_: the interface on which the event is received.
* _Message_: the message that causes the state transition.
* _Action_: the action that is executed when a state transition occurs.
* _Next state_: the state to move to after the action has been executed.

The state transition table is structured by first creating a list of states. For each state, a list of interfaces is then created on which messages can be received that can be handled in this state. Then, for each interface, a list is created of these messages. Finally, for each message, an action is defined that is executed when the message is received.

In the following sections, the structure of the state transition table is described in more detail.

## Action

An action is a function that is executed when a state transition occurs. The function has one parameter: a reference to the message that caused the state transition.

```cpp
using FcmSttAction = std::function<void(const FcmMessage&)>;
```

Inside the action, the message can be used to extract data and to send messages to other components or handlers. Note that in order to do this, the message must of course first be cast to the correct type. This is automatically done by using the ``FCM_ADD_TRANSITION`` macro as described in ["Adding a transition"](Component.md#adding-a-transition).

## Transition

A transition is a pair of an _action_ and the _next state_.

```cpp
struct FcmSttTransition
{
    FcmSttAction action;
    std::string nextState;
};
```

## Handled messages

A list of messages that are handled in a state is defined as a map with the message name as the key and the transition as the value.

```cpp
using FcmSttMessages = std::map<std::string, FcmSttTransition>;
```

## Interfaces

A list of interfaces that are handled in a state is defined as a map with the interface name as the key and the list of possible messages as the value.

```cpp
using FcmSttInterfaces = std::map<std::string, FcmSttMessages>;
```

## States

At the highest level, the state transition table is a map with the state name as the key and the list of interfaces as the value. On these interfaces, messages can be received that can be handled in this state.

```cpp
using FcmStateTransitionTable = std::map<std::string, FcmSttInterfaces>;
```

## Adding a transition

To add a transition, the ``FCM_ADD_TRANSITION`` macro is used. The macro takes five arguments: the current state, the interface, the message, the next state and the action. The action is a lambda function that is executed when the state transition occurs. Inside the action, the message can be used to extract data and to send messages to other components or handlers. This is possible because the message is automatically cast to the correct type.

```cpp
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)       \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                      \
    [this](const std::shared_ptr<FcmMessage>& msg)                              \
    {                                                                           \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
        ACTION                                                                  \
    })
```

## Choice-points

A choice-point is a special kind of state with only two transitions: "Yes" and "No" on the "Logical" interface.

The choice-point is defined by the ``FCM_ADD_CHOICE_POINT`` macro which takes two arguments: the choice-point (i.e. name) and the evaluation. The evaluation is a lambda function that is executed when the state is entered. Inside the evaluation, the state variables can be used to make a decision.

```cpp
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT,                        \
    [this]()                                            \
    {                                                   \
        EVALUATION                                      \
    })
```
