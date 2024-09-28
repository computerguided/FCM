# Logging
_In the <code>FcmBaseComponent</code> class, the logging is implemented which can be individually enabled and configured for each component, for example in the initialization of the device._

---

## Log levels

For the FCM the most important information for debugging and operational monitoring in most cases are the state transitions. Next to that are the other standard logging that can be used during development and operation:
* Info
* Warning
* Error
* Debug

## Enabling

To make it possible for the developer to enable and disable logging per component, for each level an optional log callback function is defined that is of the `FcmLogFunction` type.

```cpp
FcmLogFunction logInfoFunction;
FcmLogFunction logWarningFunction;
FcmLogFunction logErrorFunction;
FcmLogFunction logDebugFunction;
FcmLogFunction logTransitionFunction;
```

The type for the callback functions is defined as a `std::optional` because it does not have to be set for each component or for each log level.

```cpp
using FcmLogFunction = std::optional<std::function<void(const std::string& message)>>;
```

As a simple example, in the implementation of a device, a method can be specified which needs to be called to log the transitions.

```cpp
FcmLogFunction logTransition = [](const std::string& message)
{
    std::cout << message << std::endl;
};
```

This can then be used as the callback for the specific component.

```cpp
component->logTransitionFunction = logTransition;
```

## Generic prefix

In order to generate the prefix for the logging, the getLogPrefix() method must be called.

```cpp
std::string getLogPrefix(std::string logLevel) const;
```

The desired format of a logging is as follows, with the prefix the part before the message.

```
<time> - <log level> - <component name> - <message>
```

This function first needs to create a string containing a printable time.

```cpp
auto now = std::chrono::system_clock::now();
auto now_c = std::chrono::system_clock::to_time_t(now);
std::string timeString = std::ctime(&now_c);
timeString.pop_back();
```

Note that the last step is to remove the newline character from the end of the string because the `std::ctime()` function appends a newline character ('\n') to the string it returns.

With the time string created, the prefix can be returned.

```cpp
return timeString + " - " + logLevel + " - " + name + " - ";
```

## Logging functions

All logging methods are implemented in a similar way.

```cpp
void logError(const std::string& message);
void logWarning(const std::string& message);
void logInfo(const std::string& message);
void logDebug(const std::string& message);
```

Note that there is no `logTransition()` method as this is only relevant for a Functional Component and handled in its `preformTransition()` method.

To enable a specific log level for a component, the corresponding callback (as stored in the log-function attributes) as defined for the component must be set in the actual application. These are `std::optional` values, and therefore it can be checked whether the callback was set by calling the `has_value()` method.

```cpp
if (logErrorFunction.has_value())
{
    // ...Callback is set and can be called...
}
```

When the callback is set, the corresponding function can be accessed via the `value()` method. To generate the prefix for the logging, the `getLogPrefix()]` is called with the specific log level.

```cpp
logErrorFunction.value()(getLogPrefix("ERROR") + message);
```