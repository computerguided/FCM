# Device
_The system can be specified by identifying the actors, the use cases and the scenarios. Some of these actors are devices. The following describes the detailed design of the FcmDevice class._

---

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

<table>
  <tr>
   <td>Property
   </td>
   <td>Type
   </td>
   <td>Description
   </td>
  </tr>
  <tr>
   <td><code>timeStepMs</code>
   </td>
   <td><code>const uint</code>
   </td>
   <td>The time step is in milliseconds.
   </td>
  </tr>
  <tr>
   <td><code>messageQueue</code>
   </td>
   <td><code>std::shared_ptr&lt;FcmMessageQueue></code>
   </td>
   <td>The shared message queue.
   </td>
  </tr>
  <tr>
   <td><code>timerHandler</code>
   </td>
   <td><code>TimerHandler</code>
   </td>
   <td>The timer handler.
   </td>
  </tr>
  <tr>
   <td><code>components</code>
   </td>
   <td><code>std::vector&lt;std::shared_ptr&lt;FcmBaseComponent>></code>
   </td>
   <td>List of created components.
   </td>
  </tr>
</table>

## Construction

The constructor of the base class `FcmDevice` takes a single parameter, `timeStepMsParam`, which is the time step in milliseconds. This is the time step that the run-loop will use to control the rate of execution (see "_[Run loop](#run-loop)_").

```cpp
FcmDevice(int timeStepMsParam)
```

As part of the constructor, the message queue and the Timer Handler are initialized. All the other initializations are done in the derived classes by calling the overridden `initialize()` method as described further in the Initialize section. As such, the creation of a device in the main program is done by creating an instance of the derived class and calling its `initialize()` method and then calling the `run()` method.


## Initialize

The base class `FcmDevice` has a virtual method `initialize()` which must be overridden by the derived classes to perform the following initializations:

* Create components.
* Connect interfaces.
* Set reference to handlers in the relevant functional components.
* Initialize the created components.

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

## Connect components to handlers

Handlers cannot receive messages as they don’t possess a state machine. Instead, they supply a function interface to initiate asynchronous actions (e.g. start listening). To be able for a component to call these functions, the component must have a reference to the handler which must be set explicitly.

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

Inside the run-loop, the `processMessages()` method is called to handle any messages in the message queue.

```cpp
void processMessages()
```

The message queue is processed by looping through all the messages in the queue until the message queue is empty. This is done by calling the `pop()` method of the message queue, which will no longer have a value when the last message was processed.

```cpp
std::optional<std::shared_ptr<FcmMessage>> message;
while ((message = messageQueue->pop()).has_value())
{
    // ...
}
```

Every message has a `receiver` property, if not, then this represents an error condition which happens when a message is sent to an unconnected interface. In this case, an exception is thrown.

```cpp
auto receiver = (FcmComponent*)message->receiver;
auto sender = (FcmBaseComponent*)message.value()->sender;

if (receiver == nullptr)
{
    throw std::runtime_error("Component \"" + sender->name +
                            "\" sent the message \"" + message.value()->name +
                            "\" to unconnected interface \"" + message.value()->interfaceName + "\"!");
}
```

The `receiver` property of the message is a functional component which has a `processMessage()` method that can be called to process the message.

```cpp
receiver->processMessage(message.value());
```

## Run loop

The `run()` method is started after all initializations are performed.

```cpp
[[noreturn]] void run()
```

It is marked with the `[[noreturn]]` attribute, indicating that it is an infinite loop that does not return. The method starts by capturing the current time.

```cpp
auto start = std::chrono::high_resolution_clock::now();
```

An infinite loop is then entered, which will continue to run until the program is terminated.

```cpp
while (true)
{
    // ...
}
```

Inside the loop, the first operation is to call the `processMessages()` method which is responsible for processing all the messages in message queues.

```cpp
processMessages();
```

After processing the messages, the thread is put to sleep for a duration specified by `timeStepMs`.

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(timeStepMs));
```

This pause allows for a controlled rate of execution. Once the thread wakes up, the current time is again captured.

```cpp
auto end = std::chrono::high_resolution_clock::now();
```

The duration between the start and end timestamps is then calculated. This is done by subtracting `start` from `end` and casting the result to milliseconds. The result is a `std::chrono::milliseconds` object, which is a type of `std::chrono::duration`.

```cpp
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

The `duration` is then used to update the current time in the `timerHandler` by calling its `setCurrentTime()` method, after which the loop repeats.

```cpp
timerHandler.setCurrentTime(duration.count());
```

Note that setting the time at the `timerHandler` can result in "Timeout" messages.