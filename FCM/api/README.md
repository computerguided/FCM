# Functional Component Method (FCM) API Documentation

The Functional Component Method (FCM) is a structured approach designed to facilitate the development, management, and interaction of components within a software system. This document outlines the key elements of FCM, including devices, interfaces, messages, functional components, and asynchronous interface handlers.

## Device

A device represents the top-level entity in the FCM architecture. It is responsible for creating and managing all components and handlers within the system. The [FcmDevice](../doc/FcmDevice.md) class serves as the base class for all devices.

- **Creation and Management**: Devices create and manage components and handlers, ensuring proper communication and operation within the system.
- **Message Queue**: Devices utilize a main message queue for sending messages between components and handlers.

```cpp
FcmDevice::FcmDevice(int timeStepMsParam)
```

## Interfaces

Interfaces in the Functional Component Method (FCM) define the communication contracts between components or between components and handlers. They specify the set of messages that can be exchanged, facilitating a modular and decoupled design. These messages are data structures that can contain the parameters of the message. Each message is then identified by a name and is sent through an interface.

To define an interface along with its messages, you use the [FCM_SET_INTERFACE](../doc/Interfaces.md#specifying-an-interface) and [FCM_DEFINE_MESSAGE](../doc/Interfaces.md#specifying-an-interface) macros. 

As an example, consider the following definition of the interface is called `UdpEvents` which has messages `UdpMessageInd`, `EnabledInd` and `DisabledInd`.

```cpp
FCM_SET_INTERFACE(UdpEvents,

    FCM_DEFINE_MESSAGE( UdpMessageInd,
                        std::string message{}; );

    FCM_DEFINE_MESSAGE( EnabledInd );

    FCM_DEFINE_MESSAGE( DisabledInd );
);
```

This will then be replaced by:

```cpp
namespace UdpEvents
{
    const char* const currentNamespace = "UdpEvents";

    class UdpMessageInd : public FcmMessage
    {
    public:
        std::string message{};
        UdpMessageInd() { name = "UdpMessageInd"; interfaceName = currentNamespace; }
    };

    class EnabledInd : public FcmMessage
    {
    public:
        EnabledInd() { name = "EnabledInd"; interfaceName = currentNamespace; }
    }

    class DisabledInd : public FcmMessage
    {
    public:
        DisabledInd() { name = "DisabledInd"; interfaceName = currentNamespace; }
    }
}
```

In this example, the `UdpEvents` namespace is used to define the `UdpMessageInd`, `EnabledInd`, and `DisabledInd` messages. Each message is a subclass of `FcmMessage` and includes the necessary fields for communication.

## Functional Components

Functional components are the core building blocks within the Functional Component Method (FCM). They encapsulate specific behaviors of the system and interact with other components through interfaces. Each component manages its behavior using a finite-state machine, which is defined by a state transition table and choice-points.

### State Transition Table

The state transition table (STT) defines how a component reacts to messages based on its current state. It maps a combination of the current state and received message to a new state and an action to be executed during the transition.

To define transitions within a component, you use the [`FCM_ADD_TRANSITION`](../doc/Component.md#adding-a-transition) macro. Here's an example of adding a transition:

```cpp
FCM_ADD_TRANSITION("SomeState", ExampleInterface, MessageWithParams, "SomeOtherState", 
    {
        // Action to be executed during the transition
        std::cout << "Parameter 1: " << message.param1 << std::endl;
    });
```

This macro adds a transition to the component's state transition table. When the component is in "SomeState" and receives a `MessageWithParams` message through the `ExampleInterface`, it executes the specified action and transitions to "SomeOtherState".

### Choice-Points

Choice-points are decision points within the state transition table that allow for conditional branching based on dynamic conditions. A choice-point evaluates a condition and transitions to different states based on the result (typically "Yes" or "No").

To define a choice-point, you use the [`FCM_ADD_CHOICE_POINT`](../doc/Component.md#adding-a-choice-point) macro. Here's an example:

```cpp
FCM_ADD_CHOICE_POINT("CheckCondition", 
{
    return checkSomeCondition(); // Should return a boolean value
});
```

In this example, when the component reaches the "CheckCondition" choice-point, it evaluates `checkSomeCondition()`. Based on the returned boolean value, the component transitions to the next state accordingly.

By defining the state transition table and choice-points, you can create complex behaviors for functional components, allowing them to react dynamically to messages and internal conditions.

## Asynchronous Interface Handlers

Asynchronous Interface Handlers (AIH) are specialized components designed to handle asynchronous operations in a synchronous manner. They can send messages but cannot receive them directly, thus lacking a state machine.

- **Usage**: AIHs are used for operations that do not block the main execution flow, such as I/O operations.
- **Construction**: AIHs are constructed similarly to functional components but are specifically designed to handle asynchronous interfaces.

```cpp
FcmAsyncInterfaceHandler::FcmAsyncInterfaceHandler(const std::string& nameParam, ...)
```

These elements form the core of the FCM architecture, providing a structured and modular approach to software development. By leveraging devices, interfaces, messages, functional components, and asynchronous interface handlers, developers can create scalable and maintainable software systems.