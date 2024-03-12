# Messages
***
_A message is a data structure that is used to send data and events between components and handlers. A message is identified by a _name_ and is sent to a receiver on an interface_.
***

## Description

All messages are defined as subclasses from the `FcmMessage` class and have the following default fields:

| Field | Type | Description |
|:-------|:------|:-------------|
| `receiver` | `void*` | Pointer to the receiver of the message. |
| `timestamp` | `uint32_t` | Timestamp in milliseconds at which the message was sent. |
| `interfaceName` | `std::string` | Name of the interface through which the message was sent. |
| `name` | `std::string` | Name of the message. |

## Defining a message

Messages are defined within the ``namespace`` of the interface (see "[Interfaces](Interfaces.md)"). A message is defined as a class that is derived from the `FcmMessage` class. The class has a constructor that sets the `name` and `interfaceName` fields of the message. The `name` field is set to the name of the message class and the `interfaceName` field is set to the name of the interface.

This is achieved by using the `FCM_DEFINE_MESSAGE` macro. The macro takes two arguments: the name of the message and the parameters of the message.

```cpp
#define FCM_DEFINE_MESSAGE( NAME, ... )                                 \
    class NAME : public FcmMessage                                      \
    {                                                                   \
    public:                                                             \
        __VA_ARGS__                                                     \
        NAME() { name = #NAME; interfaceName = FCM_INTERFACE_NAME; }    \
    }
```

As an example, consider the following message definition:

```cpp
#undef FCM_INTERFACE_NAME
#define FCM_INTERFACE_NAME "MyInterface"

namespace MyInterface
{
    FCM_DEFINE_MESSAGE(MyMessage,
        int value{};
        std::string text{};)
}
```

This will then be replaced by:

```cpp
class MyMessage : public FcmMessage
{
public:
    int value{};
    std::string text{};
    MyMessage() { name = "MyMessage"; interfaceName = "MyInterface"; }
};
```

This defines a message class `MyInterface::MyMessage` with two fields: `value` and `text`.

Note that the uniform initialization syntax (``{}``) is used to ensure all members are initialized without specifying explicit default values. This is to prevent warnings, for example _"Constructor does not initialize these fields: value"_ when using Clang-Tidy.