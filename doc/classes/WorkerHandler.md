# Worker Handler

_The Worker Handler class is a base class that provides a simple framework to implement a worker thread._

## Description

The Worker Handler class allows for the implementation of long-running tasks without blocking the main thread. This is particularly useful for running complex algorithms that may take significant time to complete.

To start the Worker Handler, the [`start()`](#starting) method is called. This method creates a new thread and invokes the `run()` method, which must be **overridden by the subclass** to define the specific task to be performed.

The worker will continue executing until the `run()` method completes. Upon completion, a "finished" message is sent, the specifics of which are defined by the subclass via the `prepareFinishedMessage()` method, which returns a `std::shared_ptr<FcmMessage>`. For example:

```cpp
std::shared_ptr<FcmMessage> SomeWorkerHandler::prepareFinishedMessage()
{
    return std::make_shared<SomeInterface::SomeMessage>();
}
```

A running worker handler can also be canceled using the [`cancel()`](#cancelling) method. This sets the `cancelRequested` flag, and the `run()` method should regularly check this flag to determine whether to stop. Thus, `run()` is typically structured as a loop:

```cpp
while (!cancelRequested)
{
    // Perform work
}
```

In standard C++, there is no built-in method to forcefully terminate a running thread, as doing so can lead to significant issues. Instead, the Worker Handler relies on a cooperative cancellation mechanism.

The `cancel()` method guarantees that the "finished" message will not be sent if the worker is canceled. This is accomplished by removing any already sent "finished" message from the queue.

## Starting

To start the Worker Handler, call the `start()` method. This method returns `true` if the worker successfully starts, or `false` if it is already running.

```cpp
bool start();
```

The `start()` method first checks whether the worker thread is already running using the `joinable()` method. If it is, an error is logged, and the method returns `false`.

```cpp
if (workerThread.joinable())
{
    logError("Worker: " + name + " already started");
    return false;
}
```

If the worker thread is not running, the `cancelRequested` flag is set to `false` to signal that work should proceed.

```cpp
cancelRequested = false;
```

Next, a new thread is created and assigned to the `workerThread` variable:

```cpp
workerThread = std::thread(&FcmWorker::threadRun, this);
```

This assignment leverages `std::thread`'s ability to reassign thread objects. The `workerThread` variable is initially declared as an empty thread (`std::thread workerThread;`), serving as a placeholder until assigned a task.

## Threading

The Worker Handler uses a separate thread to execute the `run()` method. This thread is created in the `start()` method, which calls the `threadRun()` method:

```cpp
workerThread = std::thread(&FcmWorker::threadRun, this);
```

The `threadRun()` method begins by calling the `run()` method:

```cpp
void FcmWorkerHandler::threadRun()
{
    run();
}
```

After `run()` completes, the `threadRun()` method checks whether the `cancelRequested` flag is set. If it is, the method returns immediately:

```cpp
if (cancelRequested)
{
    return;
}
```

If `cancelRequested` is not set, `threadRun()` calls the `finished()` method, which must be overridden by the subclass for any necessary cleanup.

```cpp
finished();
```

Subsequently, `threadRun()` creates a "finished" message using `prepareFinishedMessage()`:

```cpp
finishedMessage = prepareFinishedMessage();
```

Finally, the message is sent using the `sendMessage()` method from `FcmBaseComponent`:

```cpp
sendMessage(finishedMessage);
```

## Cancelling

To cancel a running worker, call the `cancel()` method:

```cpp
void cancel();
```

This method sets the `cancelRequested` flag:

```cpp
cancelRequested = true;
```

If the worker thread is joinable, the main thread blocks until the worker thread has finished handling the cancellation request:

```cpp
if (workerThread.joinable())
{
    workerThread.join();
}
```

If a "finished" message has already been sent, it is removed from the message queue using the `removeMessage()` method, which requires a lambda function to identify the correct message:

```cpp
auto checkFunction = [this](const std::shared_ptr<FcmMessage>& msg) -> bool
{
    return msg->sender == this;
};

messageQueue.removeMessage(finishedMessage->_interfaceName, finishedMessage->_name, checkFunction);
```
