# Messages
_A message is a data structure that is used to send data and events between components and handlers. A message is identified by a name and is sent to a receiver on an interface._

## Description

All messages are defined as subclasses from the `FcmMessage` class and have the following default fields:

<table>
  <tr>
   <td>Field
   </td>
   <td>Type
   </td>
   <td>Description
   </td>
  </tr>
  <tr>
   <td><code>receiver</code>
   </td>
   <td><code>void*</code>
   </td>
   <td>Pointer to the receiver of the message.
   </td>
  </tr>
  <tr>
   <td><code>timestamp</code>
   </td>
   <td><code>int64_t</code>
   </td>
   <td>Timestamp in milliseconds at which the message was sent.
   </td>
  </tr>
  <tr>
   <td><code>interfaceName</code>
   </td>
   <td><code>std::string</code>
   </td>
   <td>Name of the interface through which the message was sent.
   </td>
  </tr>
  <tr>
   <td><code>name</code>
   </td>
   <td><code>std::string</code>
   </td>
   <td>Name (i.e. identifier) of the message.
   </td>
  </tr>
</table>

## Prepare a message

Before a component can send a message it needs to instantiate the proper class as defined in the corresponding interface file. This is done by the convenience `FCM_PREPARE_MESSAGE` macro:

```cpp
#define FCM_PREPARE_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE) \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>()
```

The created local variable as specified in the macro’s `MESSAGE` argument can be used to set the parameters of the message.

The example below shows the preparation of the "ConnectAck" message on the "Transceiving" interface which will be instantiated in the local `connectAckMessage` variable.

```cpp
FCM_PREPARE_MESSAGE(connectAckMessage, Transceiving, ConnectAck);
```

## Defining a message

Messages are defined within the namespace of the interface. A message is defined as a class that is derived from the `FcmMessage` class. The class has a constructor that sets the `name` and `interfaceName` fields of the message. The `name` field is set to the name of the message class and the `interfaceName` field is set to the name of the interface.

This is achieved by using the `FCM_DEFINE_MESSAGE` macro. The macro takes two arguments: the name of the message and the parameters of the message. The macro takes care of setting the `name` and `interfaceName` attributes of the message.

```cpp
#define FCM_DEFINE_MESSAGE(NAME, ...)                               \
    class NAME : public FcmMessage                                  \
    {                                                               \
    public:                                                         \
        __VA_ARGS__                                                 \
        NAME() { name = #NAME; interfaceName = currentNamespace; }  \
    }
```

In "[Interfaces](Interfaces.md)" an example is given on how to use this macro.

## Logical interface

To be able to send “Yes” and “No” messages to handle choice-points, the “Logical” interface is defined by default.

```cpp
FCM_SET_INTERFACE( Logical,
    FCM_DEFINE_MESSAGE( Yes );
    FCM_DEFINE_MESSAGE( No );
);
```
