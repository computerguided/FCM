# Message Queue
_Messages exchanged between components are stored first in the message queue, which is a first-in, first-out (FIFO) data structure._

## Description

The `FcmMessageQueue` class implements a thread safe message queue. To shield the queue from external manipulation, a `std::list` variable is added as a private property (see the [discussion](MessageQueue.md#discussion---using-list-instead-of-queue) for the reasons why a `list` is used instead of a `queue`).

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

## Discussion - using `list` instead of `queue`

In the current implementation, `std::list` is used instead of `std::queue`. Let’s look at the implications of this choice. Here’s a comparison of the two containers in this context:

1. **FIFO Behavior**: `std::queue` is specifically designed to operate as a first-in-first-out (FIFO) container. By contrast, `std::list` is a doubly-linked list that does not enforce FIFO but supports it by adding to the back (`push_back`) and removing from the front (`pop_front`). Since `FcmMessageQueue` strictly uses this FIFO pattern, `std::queue` would serve this purpose directly.

2. **Performance**: 
   - Both `std::queue` and `std::list` provide O(1) time complexity for adding and removing elements from either end, so performance for `push()` and `pop()` operations would be similar.
   - `std::queue` is implemented on top of a container (typically `std::deque`) that is optimized for sequential access and can outperform `std::list` in some contexts because it avoids the overhead of maintaining pointers for a doubly-linked structure.

3. **Access and Manipulation**:
   - `std::queue` does not allow direct access to elements other than the front or back. This would limit the flexibility of methods like `removeMessage()`, where the current implementation iterates through the entire list to find a specific message.
   - `std::list`, on the other hand, allows easy access to any element through iterators, making it more flexible for operations that involve accessing and modifying elements in the middle of the queue.

4. **Thread Safety**: Since `std::queue` lacks certain iterator-based access methods that `std::list` has, it might slightly reduce the risk of unintended modifications, but the current use of `std::mutex` already ensures thread safety, so there’s no additional benefit here.

5. **Use of `resendMessage()`**: The `resendMessage()` method moves a message to the front of the queue, which `std::queue` does not natively support. With `std::queue`, you’d have to either re-implement the front insertion logic or fall back on a `std::deque`.

### Conclusion
Since we need more operations that interact with elements throughout the queue, `std::list` is the better choice. If the queue’s functionality could be streamlined to just adding and removing from the ends without other element manipulation, then switching to `std::queue` (with `std::deque` as the underlying container) could be an option. However, since we do access the queue in other ways than just adding and removing from the ends, `std::list` is the better choice.
