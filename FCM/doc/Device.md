# Device
----
_The system can be specified by identifying the actors, the use cases and the scenarios. Some of these actors are devices._

----

## Table of contents

1. [Definition](#definition)
2. [Functional decomposition](#functional-decomposition)
3. [Construction](#construction)
4. [Run loop](#run-loop)
5. [Process messages](#process-messages)
6. [Copy messages](#copy-messages)
7. [Insert asynchronous message](#insert-asynchronous-message)

## Definition

Some of the actors of the system can be denoted as _devices_. On an abstract level these devices are characterized as _actors_ that can have a certain _state_ (or combination of states) and demonstrate deterministic _behavior_.

More practically, a device is defined here as follows:
* The functionality and behavior of the device is implemented in a modular way by defining a number of functional components.
* These components are specified to be interconnected in a component diagram.
* Via these connections the components can transmit messages.
* There is one shared message queue.
* No messages are lost.
* There is shared memory between these components.

## Functional decomposition
A device can be in a specific state for one use case but at the same time it could also be in another state in another use case. This could result in complex implementations if not some decomposition is performed. 

This decomposition can be done by deriving functional _components_ from the use cases and scenarios.

These components have _interfaces_ by which they are connected to one or more other components. Via these interfaces the components can send and receive messages to the connected components. As such the components can be seen as message handlers that implement a number of scenarios.

The implementation of the message handling inside a component is done by defining a _state-machine_.

Some components can be associated with functionality that includes interaction with hardware. These components have interface-ports on which hardware can 'inject' asynchronous messages to the components. This can be done via a polling-loop or via an ISR.

The components in turn can call (non-blocking) functions from the hardware API.

## Construction

1. Initialize the asynchronous message queues.
1. Construct all handlers.
1. Create all components.
1. Connect all interfaces.
1. Connect all handlers to components


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

All messages of the asynchronous message queue are now copied by calling the ``insertMessage()`` method (described in the next section) and then removing them.

```cpp
while (!messageQueue->empty())
{
    insertMessage(messageQueue->front());
    messageQueue->pop_front();
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








