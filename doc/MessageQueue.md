# Message Queue
_Messages exchanged between components are stored first in the message queue, which is a first-in, first-out (FIFO) data structure._

---

## Description

The `FcmMessageQueue` class implements a thread safe message queue. To shield the queue from external manipulation, a `std::list` variable is added as a private property.

```cpp
std::list<std::shared_ptr<FcmMessage>> queue;
```

To handle the access to the queue in order to allow asynchronous interfaces (see “_[Asynchronous interfaces](AsyncInterfaceHandler.md)_”) to add or remove messages, an `std::mutex` is defined. Note that the mutex is defined as mutable to allow it to be locked or unlocked even when operations on the object itself are logically const.

```cpp
mutable std::mutex mutex;
```

This can now be used in the queueing methods: `push()` and `pop()`. These are described in the following sections.

To be able to check messages in the queue, also a `FcmMessageCheckFunction` is defined.

```cpp
using FcmMessageCheckFunction = std::function<bool(const std::shared_ptr<FcmMessage>&)>;
```

## Singleton

There is only one message queue so the `FcmMessageQueue` can be a singleton. As such, the constructor and copy operators are made private.

```cpp
FcmMessageQueue() = default;
FcmMessageQueue(const FcmMessageQueue&) = delete;
FcmMessageQueue& operator=(const FcmMessageQueue&) = delete;
```

Furthermore, a public static `getInstance()` method is defined that can be used to get the reference to the singleton.

```cpp
static FcmMessageQueue& getInstance()
{
    static FcmMessageQueue instance;
    return instance;
}
```

## Adding a message

To add a message, the `push()` method is called, which is a wrapper around the standard `std::list::push()` method for the `queue` property.

```cpp
void FcmMessageQueue::push(const std::shared_ptr<FcmMessage>& message)
```

The first step is to lock the `mutex`.

```cpp
std::lock_guard<std::mutex> lock(mutex);
```

To add a message, first a timestamp is given to the message to indicate when it was added.

```cpp
message->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch()).count();
```

Now it can be added to the message queue.

```cpp
queue.push_back(message);
```

## Extract a message

To extract, i.e. to pop, a message from the message queue, the  `pop()` method is called, which is a wrapper around the standard `std::list::pop()` method for the `queue` property. The message returns an optional to capture the situation in which the queue is empty.

```cpp
std::optional<std::shared_ptr<FcmMessage>> pop();
```

Again, the first step is to lock the `mutex`.

```cpp
std::lock_guard<std::mutex> lock(mutex);
```

Before popping the message, first for safety, it must be checked if the queue is not empty and a `std::nullopt` must be returned if so.

```cpp
if (queue.empty()) {return std::nullopt;}
```

Now, the first step is to _move_ the shared pointer out of the list.

```cpp
auto message = std::move(queue.front());
```

This is more efficient than copying it, and it ensures that the ownership of the message is transferred to the caller without increasing the reference count temporarily.

The message is now removed from the list by calling the <code>[std::list::pop_front()](https://cplusplus.com/reference/list/list/pop_front/)</code> method.

```cpp
queue.pop_front();
```

Note that the C++ specification states that this removes the first element from the list and destroys it. However, what is actually destroyed is the shared_ptr object that was in the list. The destructor of the `shared_ptr` decrements the reference count. Since the `shared_ptr` has already been moved to `message`, the reference count is not decremented. Therefore, the `FcmMessage` object itself remains valid as long as the `message` variable holds it and can now be returned.

```cpp
return message;
```

## Remove a message

To be able to remove a specific message from the message queue, the `removeMessage()` method can be called.

```cpp
void removeMessage(const std::string& interfaceName,
                   const std::string& messageName,
                   const FcmMessageCheckFunction& checkFunction)
```

Per usual, first the mutex is locked.

```cpp
std::lock_guard<std::mutex> lock(mutex);
```

Next an iteration is started to go through the messages in the queue.

```cpp
for (auto it = queue.begin(); it != queue.end(); ++it)
{
    const auto& message = *it;
    // ...
}
```

For the current message in the loop it is checked whether the interface name and the message name correspond. If so, the supplied `checkFunction` can be executed.

```cpp
if (message->interfaceName == interfaceName && message->messageName == messageName)
{
    if (checkFunction && !checkFunction(message)) {continue;}
    queue.erase(it);
    return;
}
```

## Resend message

In some cases a message must be resent by a Functional Component in order to simplify the handling. This can be done by calling the `resendMessage()` method.

```cpp
void resendMessage( const std::shared_ptr<FcmMessage>& message)
```

This method simply places the message at the front of the queue which will cause the Functional Component that initiated this, to receive the message again.

```cpp
std::lock_guard<std::mutex> lock(mutex);
queue.push_front(message);
```