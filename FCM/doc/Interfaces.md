# Interfaces
***
_To be able to exchange data and events, components and handlers have a list of connected 'interfaces' on which messages can be sent. In this section, the concept of interfaces is explained._
***

## Description
An interface is a connection between two components or between a component and a handler. It is a way to send messages to a component. An interface is identified by a _name_ and is 'connected' to a component by a pointer to the receiver. The receiver is stored in a map with the interface name as the key. The map is part of the component or handler that sends the message.

```cpp
std::map<std::string, FcmComponent*> interfaces;
```

## Connecting interfaces

A component or handler has a method to 'connect' an interface to the component (note that handlers cannot receive messages). The method is called `connectInterface()` and has two parameters: the name of the interface and a pointer to the receiver.

```cpp
void connectInterface(const std::string& interfaceName, FcmComponent* receiver)
```

## Sending messages

A component or handler can send messages to an interface using its [`sendMessage()`](Component.md#send-a-message) method. The method has one parameter: a pointer to the message to be sent.

```cpp
void sendMessage(const std::shared_ptr<FcmMessage>& message)
```

In this method, the message is placed in the message queue that was supplied to the component or handler when it was created. The message queue is a shared resource that is used by all components and handlers in the system. The message queue is used to send messages between components and handlers. The message queue is described in the [Message Queue](MessageQueue.md) section.

Note that the `sendMessage()` method does not require the specification of the interface name. This is because the interface name is already part of the default field of the message and stored in `interfaceName`. This field is set when the message is created as also described in the next section. The message also has a field `receiver` that specifies the receiver of the message. The receiver is set to `nullptr` if the interface is not connected.

## Specifying an interface

It is good practice to define each interface in a separate header file. In the file a ``namespace`` is defined with the name of the interface. Inside this ``namespace`` the messages (see [Messages](Messages.md)) are define that can be sent on the interface.

To set the interface and define the messages the ``FCM_SET_INTERFACE`` and ``FCM_DEFINE_MESSAGE`` macros are used respectively.

```cpp
#define FCM_SET_INTERFACE(NAME, ...)                \
    namespace NAME                                  \
    {                                               \
        const char* const currentNamespace = #NAME; \
        __VA_ARGS__                                 \
    }
```

```cpp
#define FCM_DEFINE_MESSAGE(NAME, ...)                                   \
    class NAME : public FcmMessage                                      \
    {                                                                   \
    public:                                                             \
        __VA_ARGS__                                                     \
        NAME() { name = #NAME; interfaceName = currentNamespace; }    \
    }
```

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
}
```






