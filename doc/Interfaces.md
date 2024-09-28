# Interfaces
_To be able to exchange data and events, components and handlers have a list of connected 'interfaces' on which messages can be sent. In this section, the concept of interfaces is explained._

---

## Description

An interface is a connection between two components or between a component and a handler. It is a way to send messages to a component. An interface is identified by a name and is 'connected' to a component by a pointer to the receiver.


## Sending messages

A component or handler can send messages to an interface using its `sendMessage()` method which takes a reference to the message as its argument. In this method, the message is placed in the message queue that was supplied to the component or handler when it was created.

Note that the `sendMessage()` method does not require the specification of the interface name. This is because the interface name is already part of the default field of the message and stored in its `interfaceName` property. This field is set when the message is created as also described in the next section. The message also has a property `receiver` that specifies the receiver of the message. The receiver is set to `nullptr` if the interface is not connected.

## Specifying an interface

It is good practice to define each interface in a separate header file. In the file a namespace is defined with the name of the interface. Inside this namespace the messages are defined that can be sent on the interface.

To set the interface the `FCM_SET_INTERFACE` macro is defined.

```cpp
#define FCM_SET_INTERFACE(NAME, ...)                \
    namespace NAME                                  \
    {                                               \
        const char* const currentNamespace = #NAME; \
        __VA_ARGS__                                 \
    }
```

To define a message for an interface, the `FCM_DEFINE_MESSAGE` macro was defined.

```cpp
#define FCM_DEFINE_MESSAGE(NAME, ...)                               \
    class NAME : public FcmMessage                                  \
    {                                                               \
    public:                                                         \
        __VA_ARGS__                                                 \
        NAME() { name = #NAME; interfaceName = currentNamespace; }  \
    }
```

As an example, consider the following definition of the interface is called `UdpEvents` which has a message `UdpMessageInd`.

```cpp
FCM_SET_INTERFACE(UdpEvents,

    FCM_DEFINE_MESSAGE( UdpMessageInd,
                        std::string message{}; );
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

Note that the uniform initialization syntax (`{}`) is used to ensure all members are initialized without specifying explicit default values. This is to prevent warnings, for example "_Constructor does not initialize these fields: value_" when using Clang-Tidy.