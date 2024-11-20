# Device
_The system can be specified by identifying the actors, the use cases and the scenarios. Some of these actors are devices. The following describes the detailed design of the FcmDevice class._

## Description

A system can be specified by identifying the _actors_, the _use cases_ and the _scenarios_. Some of the actors of a system can be denoted as _devices_. On an abstract level these devices are characterized as having a certain state (or combination of states) and demonstrate deterministic _behavior_.

A device can be a real physical device like a network node just as well as a separate software module – like an application, service or task – that communicates with other software entities on a larger OS. More practically, a device is defined here as follows:


* The functionality and behavior of the device is implemented in a modular way by defining a number of _functional components_ that operate in a synchronized manner.
* These components are specified to be interconnected in a _component diagram_.
* The components are connected via _interfaces_.
* Via these interfaces components can exchange _messages_.
* Interfaces to external systems are implemented by asynchronous interface handlers.
* There is one shared message queue.
* There is shared memory between these components.
* The device has a _run-loop_ that processes the messages in the message queue.

A device can be in a specific state for one use case but at the same time it could also be in another state in another use case. This could result in complex implementations if not some _functional decomposition_ is performed.

This decomposition can be done by deriving _functional components_ from the use cases and scenarios. These components have _interfaces_ by which they are interconnected. Via these interfaces, components can exchange messages. As such, the components can be seen as message handlers that implement a number of scenarios.

The messages in the [shared message queue](./MessageQueue.md) are processed one-by-one. This is done by the _run-loop_ of the device. As such, message handling is done synchronously and in a run-to-completion manner.

Some components can be associated with functionality that includes handling the interaction with asynchronous parts of the device, such as IO hardware. Such components are called "Asynchronous Interface Handlers" (AIH), or handlers for short.

It is good practice to use handlers to separate the synchronous and asynchronous parts of the device. The asynchronous interaction is done by synchronizing the access to the message queue by using a mutex to allow a handler to insert its messages.

Handlers will have interfaces which can only be used to send messages but not to receive them. As such the handlers will not have a state machine. Handlers also define a set of non-blocking functions that can be called by the components.

## Class definition

The base class for a device is `FcmDevice` which has the following properties.

| Property | Type | Description |
| -------- | -------- | -------- |
| `messageQueue` | `std::shared_ptr<FcmMessageQueue>` | The shared message queue. |
| `timerHandler` | `TimerHandler` | The timer handler. |
| `components` | `std::vector<std::shared_ptr<FcmBaseComponent>>` | List of created components. |

## Construction

The constructor of the base class `FcmDevice` takes no parameters.

```cpp
FcmDevice()
```

As part of the constructor, the Message Queue and the Timer Handler are initialized. All the other initializations are done in the derived classes by calling the overridden `initialize()` method as described further in the Initialize section. As such, the creation of a device in the main program is done by creating an instance of the derived class and calling its `initialize()` method and then calling the `run()` method.

## Initialize

The base class `FcmDevice` has a virtual method `initialize()` which must be overridden by the derived classes to perform the following initializations:

1. **Define settings**
   The settings for the device are defined.
2. **Create handlers**
   All handlers are created first in order to be able to add the relevant handlers to the settings.
3. **Add relevant handlers to the settings**
   Handlers cannot receive messages as they don’t possess a state machine. Instead, they supply a function interface to initiate asynchronous actions (e.g. start listening). To be able for a functional component to call these functions, the functional component must have a reference to the handler which must be set explicitly. This is done by adding the relevant handlers to the settings.
4. **Create functional components**
   Components are created supplying the name and the settings. As described, for those functional components that require a handler, the functional component will use the reference to the handler supplied in the settings.
5. **Connect interfaces**
   Interfaces are then connected between components.
6. **Set log functions**
   The log functions are set for each component.
7. **Initialize the created components**
   Finally, the created components are initialized.

These steps are performed in the overridden `initialize()` method of the derived class and are described in the following sections.

## Create components

To be able to create a component, the `createComponent()` method is called. This method is a template method that takes the type of the component as a template parameter together with the `name` and an optional map of `settings`.

```cpp
template <class ComponentType, typename... Args>
std::shared_ptr<ComponentType> createComponent(const std::string& name,
                                               const FcmSettings& settings = {})
```

The method starts by creating a new instance of the components in which the constructor is called with the specified parameters; the name and the settings.

```cpp
auto component = std::make_shared<ComponentType>(name, settings);
```

The created component can be added to the list and then returned.

```cpp
components.push_back(component);
return component;
```

## Connecting interfaces

To connect a specific interface between two components, the `FCM_CONNECT_INTERFACE()` macro can be used.

```cpp
#define FCM_CONNECT_INTERFACE(INTERFACE, FIRST_COMPONENT, SECOND_COMPONENT) \
    connectInterface(#INTERFACE, FIRST_COMPONENT.get(), SECOND_COMPONENT.get());
```
The macro calls the `connectInterface()` method.

```cpp
void connectInterface(const std::string& interfaceName, FcmBaseComponent* firstComponent, FcmBaseComponent* secondComponent)
```

Because the component to connect to can only be a functional component, the first step is to check whether the first component is a functional component. If so, it can be connected to the second component.

```cpp
if (dynamic_cast<FcmFunctionalComponent*>(secondComponent) != nullptr)
{
    firstComponent->connectInterface(interfaceName, secondComponent);
}
```

The same is done for the second component to connect it to the first component.

```cpp
if (dynamic_cast<FcmFunctionalComponent*>(firstComponent) != nullptr)
{
    secondComponent->connectInterface(interfaceName, firstComponent);
}
```

## Initialize the created components

The last step of the initialization of the device is to initialize all the created components. This is done by calling the `initializeComponents()` method.

```cpp
void initializeComponents()
```

Since each component has an `initialize()` method, it can be called for each component in the `components` list.

```cpp
for (auto& component : components)
{
    component->initialize();
}
```

## Processing messages

The `processMessages()` method is called to handle any messages in the message queue and that takes the reference to the message as a parameter.

```cpp
void processMessages(std::shared_ptr<FcmMessage>& message)
```

Every message has a `sender` and `receiver` property.

```cpp
auto sender = (FcmBaseComponent*)message->sender;
auto receiver = (FcmComponent*)message->receiver;
```

If the receiver is not set, then this represents an error condition which happens when a message is sent to an unconnected interface.

```cpp
if (receiver == nullptr)
{
    auto errorMessage = "Sent the message \"" + message->_name +
                        "\" to unconnected interface \"" + message->_interfaceName + "\"!";
    sender->logError(errorMessage);
    return;
}
```

If the receiver is set, then the message can be processed by calling the `processMessage()` method of the receiver.

```cpp
receiver->processMessage(message);
```

## Run loop

The `run()` method is started after all initializations are performed.

```cpp
[[noreturn]] void run()
```

It is marked with the `[[noreturn]]` attribute, indicating that it is an infinite loop that does not return. 

An infinite loop is entered, which will continue to run until the program is terminated.

```cpp
while (true)
{
    // ...
}
```

Inside the loop, the first operation is to wait until a message is available in the message queue.

```cpp
auto message = messageQueue.awaitMessage();
```

When the message is available, it is passed to the `processMessages()` method which is responsible for processing the message.

```cpp
processMessages(message);
```

After processing the messages, the loop repeats.