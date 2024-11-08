# Asynchronous Interface Handler
_The Asynchronous Interface Handler is a class that provides a way to handle asynchronous interfaces in a synchronous way._

## Description

The default _Asynchronous Interface Handler_, or _handler_ for short, is defined in the `FcmAsyncInterfaceHandler` class which is a subclass of the `FcmBaseComponent` class. It provides a way to handle asynchronous interfaces in a synchronous way.

A handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

A subclass will also implement one or more so-called non-blocking interface functions which can be called by functional components that have been supplied with a reference to the handler.

Depending on the asynchronous interface, the handler implements methods to handle asynchronous events. In these event-handler methods, the handler can send messages to the interface.

## Construction

The handler is constructed by supplying a name and optional settings.

```cpp
FcmAsyncInterfaceHandler(const std::string& nameParam,
                         const FcmSettings& settingsParam = {}) :
    FcmBaseComponent(nameParam, settingsParam) {}
```

## Initialization

The base class has a virtual `initialize()` method that must be implemented by the derived class and is called by the creating device after the handler has been created.

```cpp
virtual void initialize() = 0;
```

