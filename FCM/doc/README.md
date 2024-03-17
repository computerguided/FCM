# Functional Component Method documentation
---
## Table of Contents

* [Component](#component)
* [Asynchronous Interface Handler](#asynchronous-interface-handler)
* [Device](#device)
* [Interfaces](#interfaces)
* [Messages](#messages)
* [MessageQueue](#messagequeue)
* [State Transition Table](#state-transition-table)
* [Timer Handler](#timer-handler)

## Overview

The Functional Component Method (FCM) is a comprehensive approach designed to facilitate the development, management, and interaction of components within a software system. At its core, FCM emphasizes modularity, reusability, and efficient communication between distinct functional units, known as components. This method leverages a structured framework that includes devices, message queues, state transition tables, and various handlers to streamline the process of creating scalable and maintainable software architectures. By adopting FCM, developers can create systems that are easier to debug, test, and extend, ultimately leading to more robust and flexible applications.

## Component

The `FcmComponent` is the base class for all functional components in the system. It provides a way to exchange data and events with other components and handlers. The component has a list of connected 'interfaces' on which messages can be sent. The component also has a state-machine that can be used to implement the behavior of the component.

The detailed description of the component can be found in the [Component](Component.md) section.

## Asynchronous Interface Handler

The Asynchronous Interface Handler is a class that provides a way to handle asynchronous interfaces in a synchronous way. The handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

The detailed description of the handler can be found in the [Asynchronous Interface Handler](AsyncInterfaceHandler.md) section.

## Device

The `FcmDevice` is the base class for all devices in the system. It provides a way to create and manage components and handlers. The device has a list of components and handlers that are part of the device. The device also has a message queue that is used to send messages between components and handlers.

The detailed description of the device can be found in the [Device](Device.md) section.

## Interfaces

To be able to exchange data and events, components and handlers have a list of connected 'interfaces' on which messages can be sent.

The detailed description of the interfaces can be found in the [Interfaces](Interfaces.md) section.

## Messages

A message is a data structure that is used to send data and events between components and handlers. A message is identified by a _name_ and is sent to a _receiver_ on an _interface_.

The detailed description of the messages can be found in the [Messages](Messages.md) section.

## MessageQueue

The message queue is a shared resource that is used by all components and handlers in the system. The message queue is used to send messages between components and handlers.

The detailed description of the message queue can be found in the [MessageQueue](MessageQueue.md) section.

## State Transition Table

The state transition table is a data structure that is used to define the behavior of a component. It is a table that contains the states and events of the component and the actions that are executed when a state transition occurs.

The detailed description of the state transition table can be found in the [State Transition Table](StateTransitionTable.md) section.

## Timer Handler

The Timer Handler is a class that provides a way to handle timers in the system. The handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

The detailed description of the handler can be found in the [Timer Handler](TimerHandler.md) section.


