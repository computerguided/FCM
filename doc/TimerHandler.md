# Timer Handler
_To be able to generate timeouts, the "Timer Handler" is defined which is used by the components to set timeouts._

---

## Description

The basic functionality of the `FcmTimerHandler` is to administer a list with the upcoming timeouts.

```cpp
std::multimap<long long, FcmTimerInfo> timeouts;
```

Using the `std::multimap` type ensures that the list is sorted based on the timeout, while permitting multiple entries with the same timeout.

The `FcmTimerInfo` holds the timer ID and the reference to the component that started the timer.

```cpp
struct FcmTimerInfo
{
    int timerId;
    void* component;
};
```

The timer also needs to administer the current time which can be set as explained in the next section.

```cpp
long long currentTime{};
```

To be able to send timeout messages, a reference to the message queue is stored which is set during construction.

```cpp
FcmMessageQueue& messageQueue;
```

When a new timeout is scheduled (see “Set timeout”), a unique timer ID must be returned. The next timer ID that can be used must therefore be administered.

```cpp
int nextTimerId{};
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

## Set current time

When the time progresses to a new value, the `setCurrentTime()` method of the Timer Handler must be called.

```cpp
void setCurrentTime(long long currentTimeParam)
```

The first step is of course to set the `currentTime` of the Timer Handler.

```cpp
currentTime = currentTimeParam;
```

If there are no timers running, then the method can return.

```cpp
if (timeouts.empty()) { return; }
```

With the `currentTime` set to the new value, it can be checked whether there is a timer that expired. This can be done by looping through timeouts and sending a timeout message for those that have expired. Because The list is sorted, the loop can stop when the timeout value is larger than the current time.

```cpp
auto it = timeouts.begin();
while (it != timeouts.end() && it->first <= currentTime)
{
    auto component = static_cast<FcmFunctionalComponent*>(it->second.component);
    sendTimeoutMessage(it->second.timerId, component);
    ++it;
}
```

## Set timeout

To be able to set timeouts, a component gets the reference to the Timer Handler when it is constructed. To set a new timeout, the component calls the `setTimeout()` method of the Timer Handler.

```cpp
[[nodiscard]] int setTimeout(long long timeout, void* component);
```

The component is specified as void* to prevent circular dependencies with the `FcmFunctionalComponent` includes the header file of the `FcmTimerHandler`. Also note that for the method it is indicated that the return value should not be discarded as this is the timer ID that is required to cancel the timer as described in the following section.

Because the current time is set before the new processing loop is started, it can be used to set the time at which the timeout must be generated.

```cpp
long long time = currentTime + timeout;
```

The timer info object must now be created to hold the `timerId` and the reference to the component in order to send the timeout message when the timer expires. While doing so, the `nextTimerId` can be incremented to have a unique value for the `timerId`.

```cpp
FcmTimerInfo timerInfo = {nextTimerId++, component};
```

The created timer can now be added to the list of timeouts and the created new `timerId` can be returned.

```cpp
timeouts.insert(std::make_pair(time, timerInfo));
return timerInfo.timerId;
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

Normally, a component does not have to specify the receiver for a message that it sends on a specific interface as this is already specified for the interface when it was connected (see “_[Connect an interface](#heading=h.6obm1zrex3wp)_”) and set when the message is sent by calling the components `sendMessage()` method. This is different in this case as the receiver was specified when the timeout was set.

```cpp
timeoutMessage->receiver = component;
```

Note that when the component is created the "Timer" interface is added to the list of interfaces.

As the final step, the created timeout message is added to the message queue.

```cpp
messageQueue->push(timeoutMessage);
```

## Cancel timeout

To cancel a running timer, a component must call the `cancelTimer()` method of the Timer Handler, supplying the timer ID as the argument.

```cpp
void cancelTimeout(int timerId);
```

Find the timeout with the given timer id and remove it.

```cpp
for (auto it = timeouts.begin(); it != timeouts.end(); ++it)
{
    if (it->second.timerId == timerId)
    {
        timeouts.erase(it);
        return;
    }
}
```

If the timeout was _not_ found, it is possible that the timeout has already expired but not been handled yet by the component because the timeout message is still in the message queue.

Because the component will not expect a timeout message after canceling the timer, the timeout message needs to be explicitly removed from the message queue by calling `removeTimeoutMessage()` which will be described in the next section.

```cpp
removeTimeoutMessage(timerId);
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

