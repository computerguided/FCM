# State Transition Table
_The state transition table is a data structure that is used to define the behavior of a functional component. It is a table that contains the states, the messages the functional component can react to, and the actions that are executed when a state transition occurs._

---

## Description

A _state machine_ defines the behavior of a functional component by defining a structured deterministic behavioral model in which a functional component resides in a specific _state_ and can only exit this state and move to another state due to a _message_. Moving from one state to another state is called a _transition_. It is also possible to return to the original state. This is a self-transition.

A state machine can be defined by a _state transition table_. In the FCM described here, this table comprise the following columns:

* _State_: the current state of the component.
* _Interface_: the interface on which the event is received.
* _Message_: the message that causes the state transition.
* _Action_: the action that is executed when a state transition occurs.
* _Next state_: the state to move to after the action has been executed.

The state transition table is structured by first creating a list of states. For each state, a list of interfaces is then created on which messages can be received that can be handled in this state. Then, for each interface, a list is created of these messages. Finally, for each message, an action is defined that is executed when the message is received.

In the following sections, the structure of the state transition table is described in more detail.

## Action

An _action_ is a function that is executed when a state transition occurs. The function has one parameter: a reference to the message that caused the state transition.

```cpp
using FcmSttAction = std::function<void(const FcmMessage&)>;
```

Inside the action, the message can be used to extract data and to send messages to other components or handlers. Note that in order to do this, the message must of course first be cast to the correct type. This is automatically done by using the `FCM_ADD_TRANSITION` macro.

## Transition

A transition is a pair comprising an action and the next state.

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

## Choice-point

A choice-point is a special kind of state with only two transitions: "Yes" and "No" on the "Logical" interface. By definition, the name of a choice-point ends with a question mark in order to distinguish it from a state. A choice-points has an evaluation function which performs a check based on the values of one or more of the functional components state variables and returns a boolean. This type is then used to create a dictionary which can be used to store the defined choice-points.

```cpp
using FcmSttEvaluation = std::function<bool()>;
using FcmChoicePointTable = std::map<std::string, FcmSttEvaluation>;
```