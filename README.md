# Functional Components Method

## Overview

The Functional Components Method (FCM) is a comprehensive approach designed to facilitate the development, management, and interaction of components within a software system. At its core, FCM emphasizes modularity, reusability, and efficient communication between distinct functional units, known as components. This method leverages a structured framework that includes devices, message queues, state transition tables, and various handlers to streamline the process of creating scalable and maintainable software architectures. By adopting FCM, developers can create systems that are easier to debug, test, and extend, ultimately leading to more robust and flexible applications.

The FCM supports the six best practices of software engineering by emphasizing modularity, clear definitions, visual modeling, and structured communication. This is described in the ["Software Engineering using FCM"](./doc/SoftwareEngineering.md) document.

This repository contains an implementation in C++ (for an implementation in C, refer to https://github.com/computerguided/FCM_C).

The FCM can be used to specify the software of a device. This is described in the ["FCM Specification Guide"](./doc/FcmSpecificationGuide.md). In the FCM the concept of functional components is introduced. How a functional component is derived from a Functional Component's role and behavior is described in the ["Designing Components"](./doc/DesigningComponents.md) document.

For an example see the ["Doors Controlling System"](https://github.com/computerguided/FCM-example).

In the following sections the FCM classes are described.

## Functional Component

The `FcmFunctionalComponent` is the base class for all functional components in the system. It provides a way to exchange data and events with other components. The functional component has a list of connected 'interfaces' on which messages can be both received and sent. The functional component also has a finite-state-machine that can be used to implement its behavior.

The detailed description of the component can be found in “[Functional component](./doc/FunctionalComponent)”.

## Asynchronous Interface Handler

The `FcmAsyncInterfaceHandler` is a class that provides a way to handle asynchronous interfaces in a synchronous way. The handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

Typically, Functional Components can have a reference to a handler to call non-blocking interface functions that initiate a certain asynchronous operation. The handler could then for example send a message back to the component when the operation is completed.

The detailed description of the handler can be found in “[Asynchronous Interface Handler](./doc/AsyncInterfaceHandler.md)”.

## Base Component

The `FcmBaseComponent` is the base class for the `FcmFunctionalComponent` and `FcmAsyncInterfaceHandler` classes and implements the basic attributes that are common to both classes, such as the name, settings, list of connected interfaces, and message queue. The base class also provides methods to connect interfaces, send messages, and access settings.
The detailed description of the base component can be found in “[Base Component](./doc/BaseComponent.md)”.

## Device

A device is a collection of components and handlers that work together to perform a specific function. The device is the top-level entity in the system and is responsible for creating and managing all components and handlers. The device also has a message queue that is used to send messages between components.
Note that a “device” is an abstraction; it can be a real physical device like a network node just as well as a separate software entity – like an application, service or task – that communicates with other software entities on a larger OS.
The `FcmDevice` is the base class for all devices in the system. It provides a way to create, connect and manage components and handlers. The device has a list of components and handlers that are part of the device.
The detailed description of the device can be found in “[Device](./doc/Device.md)”.

## Interfaces

To be able to exchange data and events, components and handlers have a list of connected 'interfaces' on which messages can be sent.
An interface is defined as a set of messages and typically defined in its own header file.
The detailed description of the interfaces can be found in “[Interfaces](./doc/Interfaces.md)”.

## Messages

A message is a data structure that is used to send data and events between components and handlers. A message is identified by a name and is sent to a receiver on an interface.
The detailed description of the messages can be found in the “[Messages](./doc/Messages.md)”.

## Message Queue

When a device is created, a message queue is created, which is a thread safe shared resource that is used by all components in the system. The message queue is used to send messages between components and handlers.
The detailed description of the message queue can be found in the “[Message queue](./doc/MessageQueue.md)”.

## State Transition Table

The state transition table is a data structure that is used to define the behavior of a component. It is a table that contains the states and events of the component and the actions that are executed when a state transition occurs.
The detailed description of the state transition table can be found in “[State Transition Table](./doc/StateTransitionTable.md)”.

## Timer Handler
The `FcmTimerHandler` is a class that provides a controlled way to handle timers in the system. The handler has one interface on which it can only send messages, but not receive. As such it also has no state machine. In the device there is only one timer handler, implemented as a singleton and instantiated by the `FcmDevice`.
The detailed description of the handler can be found in “[Timer Handler](./doc/TimerHandler.md)”.

## Examples
A number of small examples are available and can be found in the “[Examples](./doc/Examples.md)” section. These examples are:
* [Accessing last received message](./doc/Examples.md#accessing-last-received-message)
* [Replacing if-statements](./doc/Examples.md#replacing-if-statements)
* [Resending messages](./doc/Examples.md#resending-messages)
* [Multiple state transition](./doc/Examples.md#multiple-state-transition)
* [Creating loops](./doc/Examples.md#creating-loops)
* [Interface multiplication](./doc/Examples.md#interface-multiplication)
* [Using the timer](./doc/Examples.md#using-the-timer)
* [Handling switch statements](./doc/Examples.md#handling-switch-statements)



