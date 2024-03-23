# FCM Base Component
***
The `FcmBaseComponent` is the base class for the `FcmComponent` and `FcmAsyncInterfaceHandler` classes and implements the basic attributes that are common to both classes, such as the name, settings, list of connected interfaces, and message queue. The base class also provides methods to connect interfaces, send messages, and access settings.
***

## Table of Contents

* [Construction](#construction)
* [Connecting interfaces](#connecting-interfaces)
* [Sending messages](#sending-messages)
* [Accessing settings](#accessing-settings)

## Construction

The base component is constructed by supplying a name and a reference to a message queue. The name is used to identify the component in the system. The message queue is used to send messages to the interfaces. Also, it is possible to specify settings.

```cpp
FcmBaseComponent(const std::string& nameParam,
                 const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                 const std::map<std::string, std::any>& settingsParam);
```

The `name`, `messageQueue`, and `settings` attributes are initialized with the supplied parameters.

```cpp
std::string name;
std::shared_ptr<FcmMessageQueue> messageQueue;
std::map<std::string, std::any> settings;
```

## Connecting interfaces

A component has a map of connected interfaces. The map is a `std::map` with the interface name as the key and a pointer to the receiver as the value. The value is a pointer to a `FcmComponent` object.

```cpp
std::map<std::string, FcmComponent*> interfaces;
```

A convenience method is defined to connect an interface to a component.

```cpp
void connectInterface(const std::string& interfaceName, FcmComponent* receiver)
```

The method adds the interface to the `interfaces` map.

```cpp
interfaces[interfaceName] = receiver;
```

## Sending messages

To be able to send messages to an interface, the `sendMessage()` method can be used.

```cpp
void sendMessage(const std::shared_ptr<FcmMessage>& message)
```

First it is checked if the interface is connected, i.e. whether the interface is in the `interfaces` map. If the interface is not connected, the `receiver` field of the message is set to `nullptr` which will be handled further in message processing loop of the Device (see ["Device - Processing message"](Device.md#process-messages)).

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

With the ``receiver`` set, the message can be added to the message queue (see "[Message Queue](MessageQueue.md)") that was supplied when constructing the component (see "[Construction](#construction)").

```cpp
messageQueue->push(message);
```

## Accessing settings

Typically, the `settings` attribute can be used to set a state variable by calling the `setSetting()` template method.

```cpp
template <typename T>
void setSetting(const std::string& settingName, T& stateVariable)
```

The method will throw a `std::runtime_error` if the setting is not found or if the setting cannot be cast to the type of the state variable.

```cpp
try
{
    stateVariable = std::any_cast<T>(settings.at(settingName));
}
catch (const std::bad_any_cast& e)
{
    throw std::runtime_error("Component: " + name + " settings error: " + e.what());
}
```




