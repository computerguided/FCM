# Base Component
_The `FcmBaseComponent` is the base class for the `FcmComponent` and `FcmAsyncInterfaceHandler` classes and implements the basic attributes that are common to both classes, such as the name, settings, list of connected interfaces, and message queue. The base class also provides methods to connect interfaces, send messages, and access settings. The following describes the detailed design of the base class._

---

## Description

Within the FCM there are two types of components: `Functional Components` and `Asynchronous Interface Handlers`. For the shared functionality the `FcmBaseComponent` is defined as the base class.

A component implements a specific part of functionality of the [device](Device.md) which is reflected in the public _name_ of the component which is set at the initialization of the component.

```cpp
const std::string name;
```

Preferably the name is unique within the device but can be freely chosen. However, it is good practice to have the name reflect the component's functionality in a sense that it answers the question as to what the component does, e.g. "I am a &lt;Component Name>". Doing this makes it much easier to understand the device architecture, ensuring a modular design and preventing errors, mistakes and inefficient architectures.

The name is used to identify and display the component in the component diagram. As a guideline, when displaying the component in the documentation, the name of the component is printed in the diagram with each word capitalized and separated by a space or newline (e.g. "Connection Handler"). For the corresponding class name in the code, the spaces are omitted (e.g. `ConnectionHandler`).

A component can also have _settings_. This is a public dictionary that is offered to the component when it is instantiated, but cannot be changed, which is implemented by storing the settings as a constant attribute.

```cpp
const std::map<std::string, std::any> settings;
```

The settings are used to configure the component and typically can be used to set the initial value for the state-variables using the template method `setSettings()` which can be called in the constructor.

Depending on the design, a component can have one or more _interfaces_. It is good engineering practice to define the interfaces beforehand as much as possible. This means; defining the messages and their parameters (see "_[Messages](Messages.md)_"). The interface completely shields the component from the environment in which it is running. A more detailed description is given in "_[Interfaces](Interfaces.md)_".

Note that grouping the messages into interfaces is technically not really necessary; some state machine frameworks don't even specify interfaces, and for the method presented here, nothing prevents the developer from using only one interface containing all the messages. However, using interfaces has the following benefits:

* It creates a more modular design.
* Message names can be reused in different interfaces.
* Logging becomes easier when the interface is specified.

However, one of the most important advantages of using interfaces is that the state machine engine can be more efficient.

To store the interfaces, a protected dictionary `interfaces` attribute is defined which will be shared to the subclasses.

```cpp
std::map<std::string, std::vector<FcmBaseComponent*>> interfaces;
```

As can be seen from the definition, an interface holds a reference to `std::vector` of references to `FcmBaseComponent`. This is set when the interface is connected to the component.

In order to allow a component to send messages, a reference to the message queue is also stored in the protected `messageQueue` attribute which is of the `FcmMessageQueue` type.

```cpp
FcmMessageQueue& messageQueue;
```

## Construction

The base component is constructed by supplying a name and the (optional) settings.

```cpp
FcmBaseComponent::FcmBaseComponent(std::string nameParam,
                                   const std::map<std::string, std::any> &settingsParam = {}):
    name(std::move(nameParam)), 
    messageQueue(FcmMessageQueue::getInstance()), 
    settings(settingsParam) {}
```

## Initialization

All subclasses of `FcmBaseComponent` must define their own initialization. Therefore, the `initialize()` method is added as a virtual function.

```cpp
virtual void initialize() = 0;
```

## Connect an interface

To connect an interface to a component, a convenience method is defined.

```cpp
void connectInterface(const std::string& interfaceName, FcmComponent* receiver)
```

The first step is to check if the interfaces map already has a vector for the given interfaceName. If not, it creates one.

```cpp
auto& componentList = interfaces[interfaceName];
```

Next it is checked if the remoteComponent is already part of the vector. If this is the case, then this is a programming error.

```cpp
if (std::find(componentList.begin(), componentList.end(), remoteComponent) != componentList.end())
{
    throw std::runtime_error("Interface " + interfaceName +
                             " already connected to supplied remote component");
}
```

If the remoteComponent is not part of the vector, it can be added.

```cpp
componentList.push_back(remoteComponent);
```

This method is used when the interfaces of components are connected when the device is configured.

## Sending messages

Inside a transition, a component can send messages to other components.

A component can send a message by performing the following steps:

1. Create the message by instantiating the proper class as defined in the corresponding interface file.
2. Set the message parameters.
3. Call the `sendMessage()` method with the created message.

To help with the first step, the `FCM_PREPARE_MESSAGE` macro can be used, which exposes a properly typed local parameter with the same name as the message which can be used to set the parameters.

When all message parameters are set, the message can be sent, which can be done using the `FCM_SEND_MESSAGE` macro, with the second argument the optional index.

```cpp
#define FCM_SEND_MESSAGE(MESSAGE, ...) \
    sendMessage(MESSAGE, ##__VA_ARGS__)
```

The `sendMessage()` method can of course also be called directly, but using the macro gives more symmetrical code with the `FCM_PREPARE_MESSAGE` macro.

The `sendMessage()` method used in the macro is defined as follows.

```cpp
void sendMessage(const std::shared_ptr<FcmMessage>& message, size_t index = 0)
```

To capture programming errors, first it is checked if the interface is connected, i.e. whether the interface is in the interfaces map by using the `std::map::at()` method which will throw a `std::out_of_range` exception when the key is not found. In that case the receiver field of the message is set to `nullptr` which will be handled further in the message processing loop of the Device (see "Processing messages").

```cpp
try
{
    auto& componentList = interfaces.at(message->interfaceName);
    // ...
}
catch (const std::out_of_range& e)
{
    message->receiver = nullptr;
}
```

When the key is found, the supplied index can be checked.

```cpp
if (index >= componentList.size())
{
    throw std::out_of_range("Index out of range for interface components.");
}
message->receiver = componentList[index];
```

With the `receiver` set, the message can be added to the message queue that was supplied when constructing the component.

```cpp
messageQueue.push(message);
```

## Accessing settings

Typically, the settings attribute can be used to set a state variable by calling the `setSetting()` template method (template methods are specified in the header file).

```cpp
template <typename T>
void setSetting(const std::string& settingName, T& stateVariable)
```

To capture programming errors, the method will throw a `std::runtime_error` if the setting is not found or if the setting cannot be cast to the type of the state variable.

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

As an example, the `setSetting()` method can be used to set the `connectionTimeout` setting.

```cpp
setSetting("connectionTimeout", connectionTimeout);
```

As can be seen in this example, the setting with the key "connectionTimeout" is set to the `connectionTimeout` variable. This means that the key has the same name as the variable. In such cases it is possible to use the `FCM_SET_SETTING` macro which is defined as follows:

```cpp
#define FCM_SET_SETTING(SETTING) setSetting(#SETTING, SETTING)
```

The `connectionTimeout` setting example will then be as follows:

```cpp
FCM_SET_SETTING(connectionTimeout);
```

