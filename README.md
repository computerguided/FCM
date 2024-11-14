# Functional Components Method

## Overview

The Functional Components Method (FCM) is a comprehensive approach for developing, managing, and interacting with software components. FCM emphasizes modularity, reusability, and efficient communication between distinct functional units, called components. It uses a structured framework that includes devices, message queues, state transition tables, and handlers, aiming to create scalable and maintainable software architectures. By adopting FCM, developers can build systems that are easier to debug, test, and extend, ultimately resulting in more robust and flexible applications.

FCM supports the six best practices of software engineering by promoting modularity, clear definitions, visual modeling, and structured communication. These best practices are described in the ["Software Engineering using FCM"](./doc/SoftwareEngineering.md) document.

FCM can be used to specify the software for a device, as explained in the ["FCM Specification Guide"](./doc/FcmSpecificationGuide.md). The concept of _functional components_ is introduced in FCM, with details on deriving a functional component from its role and behavior covered in the ["Designing Components"](./doc/DesigningComponents.md) document.

For an example, see the ["Doors Controlling System"](https://github.com/computerguided/FCM-example).

This repository contains a C++ implementation (a C implementation can be found [here](https://github.com/computerguided/FCM_C)).

As a structured method, FCM allows for generating parts of the documentation and code. The [FCM Tools](https://github.com/computerguided/fcm-tools) repository provides tools for this purpose.

The following sections provide a brief overview of FCM classes. For detailed information, refer to the documentation in the [doc](./doc) folder.

For a discussion of FCM from both critics and proponents, see the ["Discussion"](#discussion) section.

## Functional Component

The `FcmFunctionalComponent` is the base class for all functional components in the system. It facilitates data and event exchange between components using connected interfaces, which can receive and send messages. It also features a finite-state machine to define component behavior.

More details can be found in ["Functional Component"](./doc/classes/FunctionalComponent.md).

## Asynchronous Interface Handler

The `FcmAsyncInterfaceHandler` provides a way to handle asynchronous interfaces synchronously. It has one or more interfaces that can send messages, but not receive them, and it does not include a state machine.

Functional Components often reference this handler to initiate non-blocking asynchronous operations. The handler can send a message back when the operation is completed.

Details are available in ["Asynchronous Interface Handler"](./doc/classes/AsyncInterfaceHandler.md).

## Worker Handler

The `FcmWorkerHandler` handles worker threads, with one or more interfaces for sending messages. It does not receive messages or include a state machine.

This handler is used to implement long-running tasks in separate threads. The task is implemented in the `run()` method and can be canceled using the `cancel()` method.

More details are in ["Worker Handler"](./doc/classes/WorkerHandler.md).

## Base Component

The `FcmBaseComponent` is the base class for `FcmFunctionalComponent` and `FcmAsyncInterfaceHandler`. It includes basic attributes common to both classes, such as name, settings, connected interfaces, and a message queue. The base class also provides methods for interface connection, message sending, and accessing settings.

More details are in ["Base Component"](./doc/classes/BaseComponent.md).

## Device

A device is a collection of components and handlers working together to perform a specific function. It serves as the top-level entity, responsible for creating and managing components and handlers. A device can be a physical entity (e.g., a network node) or a software entity (e.g., an application or service).

The `FcmDevice` class provides methods to create, connect, and manage components and handlers. It also has a message queue for communication between components.

More details are in ["Device"](./doc/classes/Device.md).

## Interfaces

Components and handlers use interfaces to exchange data and events. An interface is defined by a set of messages, typically in its own header file.

More details are in ["Interfaces"](./doc/classes/Interfaces.md).

## Messages

Messages are data structures used to send data and events between components and handlers. Each message is identified by a name and sent to a receiver via an interface.

More details are in ["Messages"](./doc/classes/Messages.md).

## Message Queue

The message queue is a thread-safe resource created when a device is instantiated. It is used by all components and handlers for message exchange.

More details are in ["Message Queue"](./doc/classes/MessageQueue.md).

## State Transition Table

The state transition table defines component behavior by listing states, events, and the corresponding actions for state transitions.

More details are in ["State Transition Table"](./doc/classes/StateTransitionTable.md).

## Timer Handler

The `FcmTimerHandler` provides a controlled way to manage timers. It has an interface to send messages, but not receive them, and does not include a state machine. The timer handler is implemented as a singleton in the `FcmDevice`.

More details are in ["Timer Handler"](./doc/classes/TimerHandler.md).

## Examples

Several examples are available in the ["Examples"](./doc/Examples.md) section:

- [Add a state transition](./doc/Examples.md#add-a-state-transition)
- [Accessing last received message](./doc/Examples.md#accessing-last-received-message)
- [Replacing if-statements](./doc/Examples.md#replacing-if-statements)
- [Resending messages](./doc/Examples.md#resending-messages)
- [Multiple state transition](./doc/Examples.md#multiple-state-transition)
- [Creating loops](./doc/Examples.md#creating-loops)
- [Interface multiplication](./doc/Examples.md#interface-multiplication)
- [Using the timer](./doc/Examples.md#using-the-timer)
- [Handling switch statements](./doc/Examples.md#handling-switch-statements)

## Discussion

![critic-proponent](https://github.com/user-attachments/assets/c65e12c9-5a13-4c22-802a-fff8229412c3)

**Critic:** The FCM framework introduces additional abstraction layers, complicating the development process. Developers may struggle with these abstractions, leading to increased complexity and inefficiencies.

**Proponent:** While FCM adds abstractions, they enhance modularity and reusability. Encapsulating functionality within well-defined components promotes separation of concerns, resulting in a more maintainable and scalable system. This structured approach reduces complexity by providing consistent component interactions.

**Critic:** The reliance on message queues and state transition tables can introduce performance bottlenecks, especially for real-time systems. The overhead from message handling and state management may not suit high-performance applications.

**Proponent:** FCM balances flexibility and performance. Though there is some overhead, it offers robust mechanisms for asynchronous communication and state management. Performance-critical applications can be optimized by fine-tuning components or integrating efficient communication protocols.

**Critic:** FCM has a steep learning curve for developers unfamiliar with its concepts, potentially extending development timelines and leading to misimplementations.

**Proponent:** Any new framework requires initial learning. FCM offers comprehensive documentation and examples to ease understanding. Once familiar, developers can leverage FCM's structured approach to streamline development and reduce time-to-market for future projects.

**Critic:** FCM's emphasis on modularity can lead to fragmented codebases, making component interconnections difficult to manage and trace.

**Proponent:** Clear interface definitions and standardized communication patterns mitigate fragmentation. By following these standards, developers maintain a cohesive codebase with transparent and traceable interactions, simplifying debugging and system comprehension.

**Critic:** Integrating FCM into existing projects may require significant refactoring, which could be resource-intensive and disrupt ongoing development.

**Proponent:** Integration should be approached incrementally, refactoring parts of the system to align with FCM principles. The long-term benefits of improved modularity and maintainability outweigh the initial effort.

**Critic:** FCM's generality might not align well with specific project needs, leading to a mismatch between the framework's capabilities and project requirements.

**Proponent:** FCM is adaptable, allowing developers to customize components and communication to fit specific needs. Its flexibility ensures it supports, rather than constrains, project-specific requirements.

**Critic:** Implementing state machines can seem redundant for straightforward system behaviors, adding unnecessary complexity.

**Proponent:** While state machines may seem excessive for simple behavior, they offer a scalable, structured approach. They improve clarity and maintainability, ensuring the system's behavior is well-defined and manageable as it evolves.

**Critic:** State machines can accumulate numerous transitions, making functionality difficult to comprehend and maintain.

**Proponent:** A large number of transitions often suggests a component is handling too much. In such cases, decomposing the component into smaller, focused units enhances clarity and maintainability, aligning with the Single Responsibility Principle.

**Critic:** Decomposing components is itself complex, requiring careful analysis to ensure consistent behavior.

**Proponent:** Thoughtful decomposition reduces complexity and improves modularity. Breaking down components facilitates easier testing, debugging, and enhancements, aligning with best software engineering practices.

**Critic:** Managing multiple state machines requires careful synchronization, which adds challenges.

**Proponent:** Proper synchronization is essential, and FCM is designed to manage such scenarios with efficient communication and coordination between components.

**Critic:** Balancing the benefits of state machines with their potential for increased complexity is crucial.

**Proponent:** Thoughtful design and adherence to best practices are key. Properly structuring components allows state machines to be effective in creating robust, maintainable, and scalable systems.

