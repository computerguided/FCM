# FCM Base Component
***
_The `FcmBaseComponent` is the base class for the `FcmComponent` and `FcmAsyncInterfaceHandler` classes and implements the basic attributes that are common to both classes, such as the name, settings, list of connected interfaces, and message queue. The base class also provides methods to connect interfaces, send messages, and access settings. The following describes the detailed design of the base class._
***

## Construction

The base component is constructed by supplying a name and a reference to a message queue. The name is used to identify the component in the system. The message queue is used to send messages to the interfaces. Also, it is possible to specify settings.

```cpp
FcmBaseComponent(const std::string& nameParam,
                 const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                 const std::map<std::string, std::any>& settingsParam);
```

The `name`, `messageQueue`, and `settings` attributes that are initialized with the supplied parameters.

```cpp
std::string name;
std::shared_ptr<FcmMessageQueue> messageQueue;
std::map<std::string, std::any> settings;
```

## Interfacing
Depending on the design, a component can have one or more _interfaces_.

It is good engineering practice to define the interfaces beforehand as much as possible. This means; defining the messages and their parameters (see "[Messages](Messages.md)").

The interface completely shields the component from the environment in which it is running. A more detailed description is given in "[Interfaces](Interfaces.md)".

Note that grouping the messages into interfaces is technically not really necessary; some state-machine frameworks don't even specify interfaces, and for the method presented here, nothing prevents the developer from using only one interface containing all the messages. However, using interfaces has the following benefits:
* It creates a more modular design.
* Message names can be reused in different interfaces.
* Logging becomes easier when the interface is specified.

However, one of the most important advantages of using interfaces is that the state-machine engine can be more efficient.

All the interfaces a component uses are defined in its dictionary ``interfaces`` attribute.

```cpp
std::map<std::string, const FcmBaseComponent*> interfaces;
```

As can be seen from the definition, an interface holds a reference to ```FcmBaseComponent``` which is set when the interface is connected to the component (see "[Connecting interfaces](#connecting-interfaces)").

## Connecting interfaces

To connect an interface to a component, a convenience method is defined.

```cpp
void connectInterface(const std::string& interfaceName, FcmComponent* receiver)
```

The method adds the interface to the `interfaces` map.

```cpp
interfaces[interfaceName] = receiver;
```

## Sending messages

To be able to send messages on an interface, the `sendMessage()` method can be used.

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




