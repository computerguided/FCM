# Device
****
_The system can be specified by identifying the actors, the use cases and the scenarios. Some of these actors are devices._
****

## Table of contents

1. [Definition](#definition)
2. [Functional decomposition](#functional-decomposition)
3. [Construction](#construction)
4. [Run loop](#run-loop)
5. [Process messages](#process-messages)
6. [Copy messages](#copy-messages)
7. [Insert asynchronous message](#insert-asynchronous-message)

## Description

A system can be specified by identifying the _actors_, the _use cases_ and the _scenarios_.

Some of the actors of the system can be denoted as _devices_. On an abstract level these devices are characterized as having a certain _state_ (or combination of states) and demonstrate deterministic _behavior_.

More practically, a device is defined here as follows:
* The functionality and behavior of the device is implemented in a modular way by defining a number of _functional components_.
* These components are specified to be interconnected in a _component diagram_.
* The components are connected via _interfaces_.
* Via these interfaces components can exchange _messages_.
* There is one shared message queue.
* There is shared memory between these components.
* The device has a _run-loop_ that processes the messages in the message queue.

## Functional decomposition
A device can be in a specific _state_ for one use case but at the same time it could also be in another state in another use case. This could result in complex implementations if not some _functional decomposition_ is performed.

This decomposition can be done by deriving _functional components_ from the use cases and scenarios.

These components have _interfaces_ by which they are interconnected. 

Via these interfaces the components can exchange messages. As such the components can be seen as message handlers that implement a number of scenarios.

The implementation of the message handling inside a component is done by defining a _state-machine_.

The messages in the shared message queue are processed one-by-one. This is done by the _run-loop_ of the device. As such, message handling is done _synchronously_ and in a [_run-to-completion_](https://en.wikipedia.org/wiki/Run_to_completion_scheduling) manner.

Some components can be associated with functionality that includes handling the interaction with _asynchronous_ parts of the device, such as hardware. Such components are called _Asynchronous Interface Handlers_ (AIH), or _handlers_ for short.

It is good practice to use handlers to separate the synchronous and asynchronous parts of the device.

The asynchronous interaction is done by having an _asynchronous message queue_ into which the handler will insert its messages. Several handlers can share an asynchronous message queue as long as it can be guaranteed that the handlers cannot insert messages at the same time (i.e. preempt each other).

Before each new run-loop, the messages in the asynchronous message queues are then moved into the main message queue.

Handlers will have interfaces which can only be used to send messages but not to receive them. As such the handlers will not have a state-machine.
Handlers also define a set of _non-blocking_ functions that can be called by the components.

## Class definition

The base class for a device is ``FcmDevice`` which has the following properties.

| Property | Type | Description |
| --- | --- | --- |
| ``timeStepMs`` | ``int`` | The time step in milliseconds. |
| ``mainMessageQueue`` | ``std::shared_ptr<FcmMessageQueue>`` | The main message queue. |
| ``messageQueues`` | ``std::vector<std::shared_ptr<FcmMessageQueue>>`` | A vector of asynchronous message queues. |
| ``timerHandler`` | ``TimerHandler`` | The timer handler. |


## Construction

The constructor of the base class ``FcmDevice`` takes a single parameter, ``timeStepMsParam``, which is the time step in milliseconds. This is the time step that the run-loop will use to control the rate of execution (see "[Run loop](#run-loop)").

```cpp
FcmDevice(int timeStepMsParam)
```

Inside the constructor, the main message queue is initialized.

```cpp
mainMessageQueue = std::make_shared<FcmMessageQueue>();
```

The Device has one timer handler which is initialized.

```cpp
timerHandler = TimerHandler();
```

All the other initializations are done in the derived classes by calling the overridden ``initialize()`` method as described further in the [Initialize](#initialize) section.

As such, the creation of a device in the main program is done by creating an instance of the derived class and calling its ``initialize()`` method and then call the ``run()`` method. This is shown in the following example.

```cpp
auto device = std::make_shared<SomeDevice>(timeStepMs);
device->initialize();
device->run();
```

## Initialize

The base class ``FcmDevice`` has a virtual method ``initialize()`` which is overridden by the derived classes to perform the following initializations:

1. Create asynchronous message queues.
1. Create handlers.
1. Create components.
1. Connect interfaces.
1. Connect handlers to components.

These steps are performed in the overridden ``initialize()`` method of the derived class and are described in the following sections.

## Create asynchronous message queues
When there are handlers in the device, one or more asynchronous message queues need to be created. This is done by calling the ``createMessageQueue()`` method which returns a shared pointer to a new message queue.

```cpp
std::shared_ptr<FcmMessageQueue> createMessageQueue()
```

In this method, the first step is to create a new message queue.

```cpp
auto messageQueue = std::make_shared<FcmMessageQueue>();
```

The message queue is then added to the vector of asynchronous message queues.

```cpp
messageQueues.push_back(messageQueue);
```

The shared pointer to the message queue is then returned.

```cpp
return messageQueue;
```

## Create handlers

## Create components

## Connect interfaces

## Connect handlers to components







## Run loop
The ``run()`` method is started after all initializations are performed.

```cpp
[[noreturn]] void run()
```

It is marked with the ``[[noreturn]]`` attribute, indicating that it is an infinite loop that does not return.  

The method starts by capturing the current time.

```cpp
auto start = std::chrono::high_resolution_clock::now();
```

An infinite loop is then entered, which will continue to run until the program is terminated.

Inside the loop, the first operation is to call the [``processMessages()``](#process-messages) method which is responsible for processing all the messages in message queues.

```cpp
processMessages();
```

After processing the messages, the thread is put to sleep for a duration specified by ``timeStepMs``.

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(timeStepMs));
```

This pause allows for a controlled rate of execution. Once the thread wakes up, the current time is again captured.

```cpp
auto end = std::chrono::high_resolution_clock::now();
```
 
The duration between the ``start`` and ``end`` timestamps is then calculated.This is done by subtracting ``start`` from ``end`` and casting the result to milliseconds. The result is a ``std::chrono::milliseconds`` object, which is a type of ``std::chrono::duration``.

```cpp
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

The ``duration`` is then used to update the current time in the ``timerHandler`` by calling its [``setCurrentTime()``](TimerHandler.md#set-current-time) method.

```cpp
timerHandler.setCurrentTime(duration.count());
```
Note that setting the time at the ``timerHandler`` can result in "Timeout" messages.

The loop then repeats. 

## Process messages
Inside the run-loop, the ``processMessages()`` method is called to handle any messages in the message queue.

```cpp
void processMessages()
```

The first step is to loop through all the asynchronous message queues and copy all their messages into the main message queue.

```cpp
for (auto& messageQueue : messageQueues)
{
    copyMessages(messageQueue);
}
```

After copying all the messages, the main message queue is processed. This is done by looping through all the messages in the queue and calling the ``processMessage()`` method of the receiver of the message. If the receiver is not found, an error is logged and the loop continues.

```cpp
while (!mainMessageQueue->empty())
{
    auto message = mainMessageQueue->front();
    auto receiver = (FcmComponent*)message->receiver;

    if (receiver == nullptr)
    {
        // TODO: Log error.
        continue;
    }

    receiver->processMessage(*message);
    mainMessageQueue->pop_front();
}
```

## Copy messages

While copying messages from an asynchronous message queue to the main message queue, the ``copyMessages()`` method was called.

```cpp
void copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue)
```

All messages of the asynchronous message queue are popped from the specified message queue and copied into the main message queue. The timestamp of the message is updated to the current time in milliseconds.

```cpp
while (!messageQueue->empty())
{
    auto message = messageQueue->pop();

    message->timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    mainMessageQueue->pop();
}
```

## Insert asynchronous message

To insert an asynchronous message into the main message queue, the ``insertMessage()`` method is called.

```cpp
void insertMessage(const std::shared_ptr<FcmMessage>& message)
```

To keep the messages queue ordered in ascending timestamp, the location is searched where the message already in the main message queue has a larger timestamp than the message to be added. This is done using a lambda expression.

```cpp
auto it = std::find_if(mainMessageQueue.begin(), mainMessageQueue.end(),
    [&message](const std::shared_ptr<FcmMessage>& m)
    {
        return m->timestamp > message->timestamp;
    });
```

With the location found, the message can be inserted.

```cpp
mainMessageQueue.insert(it, message);
```








