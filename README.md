# Functional Components Method

## Overview

The Functional Components Method (FCM) is a comprehensive approach designed to facilitate the development, management, and interaction of components within a software system. At its core, FCM emphasizes modularity, reusability, and efficient communication between distinct functional units, known as components. This method leverages a structured framework that includes devices, message queues, state transition tables, and various handlers to streamline the process of creating scalable and maintainable software architectures. By adopting FCM, developers can create systems that are easier to debug, test, and extend, ultimately leading to more robust and flexible applications.

The FCM supports the six best practices of software engineering by emphasizing modularity, clear definitions, visual modeling, and structured communication. This is described in the ["Software Engineering using FCM"](./doc/SoftwareEngineering.md) document.

The FCM can be used to specify the software of a device. This is described in the ["FCM Specification Guide"](./doc/FcmSpecificationGuide.md). In the FCM the concept of _functional components_ is introduced. How a functional component is derived from it's role and behavior is described in the ["Designing Components"](./doc/DesigningComponents.md) document.

For an example see the ["Doors Controlling System"](https://github.com/computerguided/FCM-example).

This repository contains an implementation in C++ (for an implementation in C, refer to https://github.com/computerguided/FCM_C). In the following sections the FCM classes are described.

Since the FCM is a structured method, it is possible to generate parts of the documentation and code. The repository [FCM Tools](https://github.com/computerguided/fcm-tools) contains a set of tools to do just that.

The following sections give a short description of the FCM classes. For more detailed information, please refer to the documentation in the [doc](./doc) folder.

For a discussion on the FCM from both critics and proponents see the ["Discussion"](#discussion) section.

## Functional Component

The `FcmFunctionalComponent` is the base class for all functional components in the system. It provides a way to exchange data and events with other components. The functional component has a list of connected 'interfaces' on which messages can be both received and sent. The functional component also has a finite-state-machine that can be used to implement its behavior.

The detailed description of the component can be found in “[Functional component](./doc/classes/FunctionalComponent.md)”.

## Asynchronous Interface Handler

The `FcmAsyncInterfaceHandler` is a class that provides a way to handle asynchronous interfaces in a synchronous way. The handler has one or more interfaces on which it can only send messages, but not receive. As such it also has no state-machine.

Typically, Functional Components can have a reference to a handler to call non-blocking interface functions that initiate a certain asynchronous operation. The handler could then for example send a message back to the component when the operation is completed.

The detailed description of the handler can be found in “[Asynchronous Interface Handler](./doc/classes/AsyncInterfaceHandler.md)”.

## Base Component

The `FcmBaseComponent` is the base class for the `FcmFunctionalComponent` and `FcmAsyncInterfaceHandler` classes and implements the basic attributes that are common to both classes, such as the name, settings, list of connected interfaces, and message queue. The base class also provides methods to connect interfaces, send messages, and access settings.
The detailed description of the base component can be found in “[Base Component](./doc/classes/BaseComponent.md)”.

## Device

A device is a collection of components and handlers that work together to perform a specific function. The device is the top-level entity in the system and is responsible for creating and managing all components and handlers. The device also has a message queue that is used to send messages between components.
Note that a “device” is an abstraction; it can be a real physical device like a network node just as well as a separate software entity – like an application, service or task – that communicates with other software entities on a larger OS.
The `FcmDevice` is the base class for all devices in the system. It provides a way to create, connect and manage components and handlers. The device has a list of components and handlers that are part of the device.
The detailed description of the device can be found in “[Device](./doc/classes/Device.md)”.

## Interfaces

To be able to exchange data and events, components and handlers have a list of connected 'interfaces' on which messages can be sent.
An interface is defined as a set of messages and typically defined in its own header file.
The detailed description of the interfaces can be found in “[Interfaces](./doc/classes/Interfaces.md)”.

## Messages

A message is a data structure that is used to send data and events between components and handlers. A message is identified by a name and is sent to a receiver on an interface.
The detailed description of the messages can be found in the “[Messages](./doc/classes/Messages.md)”.

## Message Queue

When a device is created, a message queue is created, which is a thread safe shared resource that is used by all components in the system. The message queue is used to send messages between components and handlers.
The detailed description of the message queue can be found in the “[Message queue](./doc/classes/MessageQueue.md)”.

## State Transition Table

The state transition table is a data structure that is used to define the behavior of a component. It is a table that contains the states and events of the component and the actions that are executed when a state transition occurs.
The detailed description of the state transition table can be found in “[State Transition Table](./doc/classes/StateTransitionTable.md)”.

## Timer Handler
The `FcmTimerHandler` is a class that provides a controlled way to handle timers in the system. The handler has one interface on which it can only send messages, but not receive. As such it also has no state machine. In the device there is only one timer handler, implemented as a singleton and instantiated by the `FcmDevice`.
The detailed description of the handler can be found in “[Timer Handler](./doc/classes/TimerHandler.md)”.

## Examples
A number of small examples are available and can be found in the “[Examples](./doc/Examples.md)” section. These examples are:
* [Add a state transition](./doc/Examples.md#add-a-state-transition)
* [Accessing last received message](./doc/Examples.md#accessing-last-received-message)
* [Replacing if-statements](./doc/Examples.md#replacing-if-statements)
* [Resending messages](./doc/Examples.md#resending-messages)
* [Multiple state transition](./doc/Examples.md#multiple-state-transition)
* [Creating loops](./doc/Examples.md#creating-loops)
* [Interface multiplication](./doc/Examples.md#interface-multiplication)
* [Using the timer](./doc/Examples.md#using-the-timer)
* [Handling switch statements](./doc/Examples.md#handling-switch-statements)

## Discussion
![critic-proponent](https://github.com/user-attachments/assets/c65e12c9-5a13-4c22-802a-fff8229412c3)

**Critic:** The Functional Component Method (FCM) framework introduces additional layers of abstraction, which can complicate the development process. Developers might find it challenging to navigate through these abstractions, leading to increased complexity and potential inefficiencies.

**Proponent:** While FCM does introduce abstractions, they are designed to enhance modularity and reusability. By encapsulating functionality within well-defined components, FCM promotes a clear separation of concerns, making the system more maintainable and scalable. This structured approach can ultimately reduce complexity by providing a consistent framework for component interaction.

**Critic:** The reliance on message queues and state transition tables in FCM could introduce performance bottlenecks, especially in systems requiring real-time processing. The overhead associated with message handling and state management might not be suitable for high-performance applications.

**Proponent:** FCM's design aims to balance flexibility and performance. While message queues and state transition tables add some overhead, they also provide robust mechanisms for asynchronous communication and state management. For performance-critical applications, FCM allows for optimization by fine-tuning these components or integrating more efficient communication protocols as needed.

**Critic:** The learning curve associated with FCM can be steep for developers unfamiliar with its concepts. This could lead to longer development times and potential misimplementation of the framework's principles.

**Proponent:** Adopting any new framework requires an initial investment in learning. FCM provides comprehensive documentation and examples to assist developers in understanding its concepts. Once familiar with FCM, developers can leverage its structured approach to streamline development processes, potentially reducing time-to-market for future projects.

**Critic:** FCM's emphasis on modularity might lead to fragmented codebases, where the interconnections between components become difficult to manage and trace. This fragmentation could hinder debugging and system comprehension.

**Proponent:** FCM encourages clear interface definitions and standardized communication patterns between components, which can mitigate fragmentation. By adhering to these standards, developers can maintain a cohesive codebase where component interactions are transparent and traceable, facilitating easier debugging and system understanding.

**Critic:** Incorporating FCM into existing projects may require significant refactoring, which could be resource-intensive and disrupt ongoing development.

**Proponent:** Integrating FCM into existing projects should be approached incrementally. By gradually refactoring parts of the system to align with FCM principles, teams can manage the transition without major disruptions. The long-term benefits of improved modularity and maintainability can outweigh the initial refactoring efforts.

**Critic:** The generality of FCM might not align with the specific needs of certain projects, leading to a mismatch between the framework's capabilities and the project's requirements.

**Proponent:** FCM is designed to be adaptable, allowing developers to customize components and communication mechanisms to fit specific project needs. Its flexibility enables it to be tailored to various application domains, ensuring that the framework supports, rather than constrains, project-specific requirements.

**Critic:** Implementing state machines can sometimes appear redundant, especially when the system's behavior is straightforward. In such cases, the added complexity of defining states and transitions may not be justified.

**Proponent:** While it's true that for simple behaviors, state machines might seem unnecessary, they provide a structured approach that can scale with system complexity. Even in simple cases, using state machines can enhance clarity and maintainability, ensuring that as the system evolves, its behavior remains well-defined and manageable.

**Critic:** However, as state machines grow, they can accumulate numerous transitions, which can obfuscate the system's functionality. A complex state machine with many transitions can become difficult to understand and maintain.

**Proponent:** An increase in the number of transitions often indicates that a component is handling too much functionality. This is akin to a function becoming too long or a class encompassing too many responsibilities. In such scenarios, it's advisable to decompose the component into smaller, more focused units. This modular approach aligns with the Single Responsibility Principle, enhancing both clarity and maintainability.

**Critic:** But isn't decomposing components into smaller units a complex task in itself? It requires careful analysis to ensure that the new components interact correctly and that the system's overall behavior remains consistent.

**Proponent:** Decomposing components does require thoughtful design, but the benefits outweigh the initial effort. By breaking down a complex state machine into smaller, more manageable components, each with its own state machine, we can reduce complexity and improve the system's modularity. This approach facilitates easier testing, debugging, and future enhancements. Moreover, it aligns with best practices in software engineering, such as modular design and separation of concerns.

**Critic:** Still, managing multiple state machines and ensuring their correct interaction can introduce its own set of challenges. Coordinating between different components and their respective states requires careful synchronization and communication.

**Proponent:** Indeed, coordinating multiple state machines necessitates a well-defined communication strategy. However, frameworks like the Functional Component Method (FCM) are designed to handle such scenarios. FCM provides mechanisms for efficient communication and synchronization between components, ensuring that their interactions are seamless and that the system's overall behavior is coherent. By leveraging such frameworks, we can manage the complexity associated with multiple state machines effectively.

**Critic:** In summary, while state machines offer a structured approach to modeling system behavior, they can become complex and unwieldy if not managed properly. It's crucial to balance the benefits of using state machines with the potential for increased complexity, ensuring that they are applied judiciously and that components are appropriately decomposed to maintain clarity and maintainability.

**Proponent:** Absolutely. The key lies in thoughtful design and adherence to best practices. By carefully considering the system's requirements and structuring components appropriately, we can harness the power of state machines to create robust, maintainable, and scalable systems. It's about finding the right balance and ensuring that the tools and methodologies we use serve to simplify and clarify the system, rather than adding unnecessary complexity.



