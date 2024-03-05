# Asynchronous Interface Handler
****
_The Asynchronous Interface Handler is a class that provides a way to handle asynchronous interfaces in a synchronous way._
****

## Table of Contents

## Description
The Asynchronous Interface Handler, or _handler_ for short, is implemented in the `FcmAsyncInterfaceHandler` class and provides a way to handle asynchronous interfaces in a synchronous way.

A handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

The base class has a virtual initializer method that must be implemented by the derived class and is called by the creating device after the handler has been created.

A subclass will also implement one or more so-called _interface functions_ which can be called by functional components that have been supplied with a reference to the handler.

Depending on the asynchronous interface, the handler implements methods to handle asynchronous events. In these event-handler methods, the handler can send messages to the interface as will be explained in "[Sending messages](#sending-messages)".

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



