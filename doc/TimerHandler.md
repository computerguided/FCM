# Timer Handler
---
_To handle timeouts, the "Timer Handler" is defined which is used by the components to set timeouts._

---
## Description

The basic functionality of the ``FcmTimerHandler`` is to administer a list with the upcoming timeouts that hold the references to the receivers.

```cpp
std::multimap<long long, void*> timeouts;
```

Using the [``std::multimap``](https://en.cppreference.com/w/cpp/container/multimap) type ensures that the list is sorted based on the timeout, while permitting multiple entries with the same timeout.




## Set current time
When the time progressed to a new value (see "[](Device.md)"), the ``setCurrentTime()`` method of the Timer Handler must be called.

```cpp
