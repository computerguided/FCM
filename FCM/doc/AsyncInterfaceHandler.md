# Asynchronous Interface Handler
****
_The Asynchronous Interface Handler is a class that provides a way to handle asynchronous interfaces in a synchronous way._
****

## Table of Contents

## Description



## Sending messages
A handler can send messages to an interface using the `sendMessage()` method.

```cpp
void sendMessage(const std::shared_ptr<FcmMessage>& message)
```

First it is checked if the interface is connected, i.e. whether the interface is in the `interfaces` map. If the interface is not connected, the ``receiver`` field of the message is set to `nullptr` to allow the message to be processed later.

```cpp
try
{
    message->receiver = interfaces.at(message->interfaceName);
}
catch (const std::out_of_range& e)
{
    message->receiver = nullptr;
}
```

With this set, the message then pushed to the message queue.

```cpp
messageQueue->push(message);
```



