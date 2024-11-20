# FcmTimerHandler
_To be able to generate timeouts, the `FcmTimerHandler` class is defined which is used by the components to set timeouts._

## Description

The `FcmTimerHandler` is implemented as a singleton and used by the components to set timeouts. To do this, it offers the following methods to the components.

- `setTimeout()` to set a new timeout.
- `cancelTimeout()` to cancel a timeout.

Each timer will run in its own thread, sleep for the specified duration, and then invoke `sendTimeoutMessage()`.

To cancel a timer, the `cancelTimeout()` method can be called. This will set a `cancelled` flag for the timer which will be checked when the timer thread wakes up and will prevent the timeout message from being sent when the flag is set.

## Timer info

The `FcmTimerInfo` structure is used to hold the information of a timer. It has the following fields.

| Field | Type | Description |
|:-----|:-----|:------------|
| `component` | `void*` | The reference to the component that started the timer, which is needed to send the timeout message. |
| `cancelled` | `bool` | Indicates whether the timer has been cancelled. |

The structure is defined as follows.

```cpp
struct FcmTimerInfo
{
    void* component;
    bool cancelled;
};
```

A `std::unordered_map` is used to store the timers where the timer ID is the unique key and the timer information is the value.

```cpp
std::unordered_map<int, FcmTimerInfo> timeouts;
```

## Set timeout

To be able to set timeouts, a component gets the reference to the `FcmTimerHandler` when it is constructed. To set a new timeout, the component calls the `setTimeout()` method of the Timer Handler.

```cpp
[[nodiscard]] int setTimeout(FcmTime timeout, void* component);
```

The component is specified as void* to prevent circular dependencies with the `FcmFunctionalComponent` includes the header file of the `FcmTimerHandler`. Also note that for the method it is indicated that the return value should not be discarded as this is the timer ID that is required to cancel the timer as described in the following section.

The first step is to create a unique timer ID. This is done by incrementing the `nextTimerId` which is a static member of the `FcmTimerHandler`.

```cpp
int timerId = nextTimerId++;
```

Then the timer information is created. This must be done in a thread-safe manner and therefore the mutex is used. Using the `emplace()` method is preferred over using the `insert()` method as it constructs the `FcmTimerInfo` object directly in the map, which avoids the need for a copy or move assignment, which would not work as the `FcmTimerInfo` object is not copyable.

```cpp   
std::lock_guard<std::mutex> lock(mutex);
timeouts.emplace(std::make_pair(timerId, FcmTimerInfo{component, false, std::thread()}));
```

Next, the timer thread is started, which is detached to keep running after the function returns.

```cpp
std::thread([this, timerId, timeout]() 
{
    // ...
}).detach();
```

Inside the thread lambda function, the thread will sleep for the duration of the timer.

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
```
When the timer expires, the first thing that is done is to lock the mutex to make sure that the timer information is not changed while it is being accessed.

```cpp
std::lock_guard<std::mutex> lock(mutex);
```

If the timer has not been cancelled, the timeout message is sent.

```cpp
if (!timeouts[timerId].cancelled.load())
{
    sendTimeoutMessage(timerId, timeouts[timerId].component);
}
```

After this is done, the element is erased from the `timeouts` map.

```cpp
timeouts.erase(timerId);
```

## Cancel timeout

To cancel a running timer, a component must call the `cancelTimer()` method of the Timer Handler, supplying the timer ID as the argument.

```cpp
void cancelTimeout(int timerId);
```

The first step is to lock the mutex to make sure that the timer information is not changed while it is being accessed.

```cpp
std::lock_guard<std::mutex> lock(mutex);
```

Next, it must be checked whether the timer information exists. If so, the `cancelled` flag is set to `true` and the method returns.

```cpp
if (timeouts.find(timerId) != timeouts.end())
{
    timeouts[timerId].cancelled = true;
    return;
}
```

However, if the timer information does not exist, the timer has already expired and the timeout message has already been sent, i.e. is in the message queue. Because the component will not expect a timeout message after canceling the timer, the timeout message needs to be explicitly removed from the message queue by calling `removeTimeoutMessage()`.

```cpp
removeTimeoutMessage(timerId);
```

## Singleton

There is only one Timer Handler so the `FcmTimerHandler` can be a singleton. As such, the constructor and copy operators are made private.

```cpp
FcmTimerHandler() : messageQueue(FcmMessageQueue::getInstance()) {}
FcmTimerHandler(const FcmTimerHandler&) = delete;
FcmTimerHandler& operator=(const FcmTimerHandler&) = delete;
```

Note that the Timer Handler needs the reference to the message queue, which is set in the private constructor. The reference to the message queue is required to be able to transmit the “Timeout” messages on the “Timer” interface described in the next section.

A public static `getInstance()` method is defined that can be used to get the reference to the singleton.

```cpp
static FcmTimerHandler& getInstance()
{
    static FcmTimerHandler instance;
    return instance;
}
```

## Timer interface

To be able to send timeout messages, the “Timer” interface is defined which includes a “Timeout” message that has the timerId as parameter.

```cpp
FCM_SET_INTERFACE(Transceiving, FCM_DEFINE_MESSAGE( Timeout, int timerId{}; )
```

## Send timeout message

When a timer expires, the “Timeout” message must be sent on the “Timer” interface. This is done by calling the `sendTimeoutMessage()` method.

```cpp
void sendTimeoutMessage(int timerId, void* component)
```

The first step is preparing the “Timeout” message that is to be transmitted on the “Timer” interface. This can be done using the `FCM_PREPARE_MESSAGE` macro.

```cpp
FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
```

As specified, the macro creates the timeoutMessage for which the `timerId` parameter can be set.

```cpp
timeoutMessage->timerId = timerId;
```

Normally, a component does not have to specify the receiver for a message that it sends on a specific interface as this is already specified for the interface when it was connected and set when the message is sent by calling the components `sendMessage()` method. This is different in this case as the receiver was specified when the timeout was set.

```cpp
timeoutMessage->receiver = component;
```

Note that when the component is created the "Timer" interface is added to the list of interfaces.

As the final step, the created timeout message is added to the message queue.

```cpp
messageQueue->push(timeoutMessage);
```

## Remove timeout message

In order to explicitly remove a timeout for a specific timer ID from the message queue, the `removeTimeoutMessage()` method must be called.

```cpp
void removeTimeoutMessage(int timerId)
```

Explicitly removing messages from the message queue can be done by calling the `removeMessage()` method of the message queue which requires a reference to a function to check the parameter of the message.

The first step therefore is to specify this `checkFunction` lambda function. This function expects a “Timeout” message and consequently checks  the `timerId`.

```cpp
auto checkFunction = [timerId](const std::shared_ptr<FcmMessage>& msg) -> bool
{
    auto timeoutMessage = dynamic_cast<const Timer::Timeout*>(msg.get());
    return timeoutMessage && timeoutMessage->timerId != timerId;
};
```

This lambda function can now be used together with the timerId to explicitly remove the timeout message.

```cpp
messageQueue.removeMessage("Timer", "Timeout", checkFunction);
```

