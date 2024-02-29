# Timer Handler
---
_To be able to generate timeouts, the "Timer Handler" is defined which is used by the components to set timeouts._

---

# Table of contents

1. [Description](#description)
2. [Set current time](#set-current-time)
3. [Set timeout](#set-timeout)
4. [Cancel timeout](#cancel-timeout)
5. [Send timeout message](#send-timeout-message)
6. [Remove timeout message](#remove-timeout-message)

## Description

The basic functionality of the ``FcmTimerHandler`` is to administer a list with the upcoming timeouts.

```cpp
std::multimap<long long, FcmTimerInfo> timeouts;
```

Using the [``std::multimap``](https://en.cppreference.com/w/cpp/container/multimap) type ensures that the list is sorted based on the timeout, while permitting multiple entries with the same timeout.

The ``FcmTimerInfo`` holds the timer ID and the reference to the component that started the timer,

```cpp
struct FcmTimerInfo
{
    int timerId;
    void* component;
};
```

## Set current time
When the time progressed to a new value (see "[Device](Device.md)"), the ``setCurrentTime()`` method of the Timer Handler must be called.

```cpp
void setCurrentTime(long long currentTimeParam)
```

The first step is of course to set the ``currentTime`` of the Timer Handler.

```cpp
currentTime = currentTimeParam;
```

If there are no timers running, then the method can return.

```cpp
if (timeouts.empty()) { return; }
```

With the ``currentTime`` set to the new value, it can be checked whether there are timer that expired. This can be done by looping through ``timeouts`` and [send a timeout message](#send-timeout-message) for those that have expired. because The list is sorted, the loop can stop when the timeout value is larger than the current.

```cpp
auto it = timeouts.begin();
while (it != timeouts.end() && it->first <= currentTime)
{
    auto component = static_cast<FcmComponent*>(it->second.component);
    sendTimeoutMessage(it->second.timerId, component);
}
```

## Set timeout

To be able to set timeouts, a component gets the reference to the Timer Handler when it is constructed (see "[Component construction](Component.md#component-construction)").

To set a new timeout, the component calls the ``setTimeout()`` method of the Timer Handler.

```cpp
[[nodiscard]] int setTimeout(long long timeout, void* component);
```
Note that for the method it is indicated that the return value should not be discarded as this is the timer ID that is required to cancel the timer as described in the following section.

The time at which the timeout must be generated is set.
    
```cpp   
long long time = currentTime + timeout;
```

The timer info object must be created to hold the ``timerId`` and the reference to the component.

```cpp
FcmTimerInfo timerInfo = {nextTimerId++, component};
```

This can now be added to the list of timeouts.

```cpp
timeouts.insert(std::make_pair(time, timerInfo));
```

Finally, the created new ``timerId`` can be returned.
    
```cpp
return timerInfo.timerId;
```

## Cancel timeout

To cancel a running timer, the component must call the ``cancelTimer()`` method of the Timer Handler, supplying the timer ID as the argument.


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
        return; // Found it, erase it, so we can return.
    }
}
```

If the timeout was not found, it is possible that the timeout has already expired but not been handled yet by the component because the timeout message is still in the message queue. Therefore it needs to be removed from the message queue by calling [``removeTimeoutMessage()``](#remove-timeout-message).

```cpp
removeTimeoutMessage(timerId);
```

## Send timeout message

```cpp
void sendTimeoutMessage(int timerId, void* component)
```

Create a timeout message.

```cpp
FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
```

Set the ``timerId`` parameter of the created message.

```cpp
timeoutMessage->timerId = timerId;
```

Have the component send it to itself. Note that when the component is created (see "[Component construction](Component.md#component-construction)") the "Timer" interface is connected to the component by default.

```cpp
auto* castedComponent = static_cast<FcmComponent*>(component);
castedComponent->sendMessage(timeoutMessage);
```

## Remove timeout message

```cpp
void removeTimeoutMessage(int timerId);
```

Search the message queue for the timeout message and remove it.

```cpp
for (auto it = messageQueue->begin(); it != messageQueue->end(); ++it)
{
    ...
}
```

Any messages that are not on the "Timer" interface can be skipped.

```cpp
if ((*it)->interfaceName != "Timer") {continue;}
```
Since there is only one message on the "Timer" interface, the found message can be cast to "Timeout" message to access the timer id.

If the "Timeout" message does not contain the correct timer ID, it can be skipped.

```cpp
auto timeoutMessage = dynamic_cast<Timer::Timeout*>(it->get());
if (timeoutMessage->timerId != timerId) { continue;}
```

If the "Timeout" message has the correct timer id, it can be removed.

```cpp
messageQueue->erase(it);
return;
```