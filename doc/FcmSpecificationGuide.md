# Functional Components Method Specification Guide

_The Functional Components Method is used to specify the software of a device. This guide provides best practices for implementing it._

## Use Cases

The system's functionality is driven by specific _objectives_. Examples of system objectives include:

- Establishing connections
- Storing files
- Monitoring connections
- Reading sensor data
- etc.

To achieve an objective, different _actors_ are involved. An _actor_ is any entity that can perform an action within the system, such as:

- A human operator
- Another device
- A sensor
- etc.

A _use case_ is defined by its objective and the actors involved. Use cases may also have dependencies, meaning certain objectives must be met before others can be achieved.

A _use case diagram_ is used to show which actors are involved in each use case, along with the dependencies among use cases. An example is provided in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuR9oJ2x9Br9mj5BGrRLJq0WjJbL8JWGIZfeufPpXKawuTM58EU6MmcEW2rpA6RU1pYJe0jg5x5nCCnrIyrA00GC0)

## Scenarios

Use cases indicate which actors need to cooperate to achieve a specific objective. _Scenarios_ describe how these actors cooperate to fulfill a use case.

### Definition

Scenarios define the sequence of events and actions required to achieve a use case objective. There may be multiple scenarios for achieving an objective, including scenarios that handle failures when required events do not occur.

Each scenario has a _precondition_, describing the situation at the start. Scenarios describe the communication between actors, which can include:

- Completion of actions
- Notifications of events
- Requests or commands
- Responses to requests

The sequence of events and actions is depicted in a _sequence diagram_.

### Simple Sequence Diagram

If an actor can respond immediately and no history or state change occurs, a simple sequence diagram is used, as illustrated in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuL9oJ2x9Br9mL5BGjLE0SfoKh1G2ClFIkU1y94b7a6G-KD8XDIy5Q080)

### Actions

When an actor needs to take action in response to an event, this can be represented with a comment box in the sequence diagram, as shown below.

![](https://www.plantuml.com/plantuml/img/RSz12W8n343HVKyHkjOBp4AcwiPZn3HgiAP39wFWwQqeP3EhG7Xyad4sRBOyIfZw5989rI0cijeWnRNjFeR65wXFRWvXW9EA2HPveqdL5RFnGkU7gihqfS3zY7EH-SOvU76xSpySqH14iz2lxYLdwS-GxbfVXVELmyYQ-tSV)

### Referencing

Dependencies between use cases are shown in the use case diagram, but it may also be helpful to reference other scenarios in a sequence diagram. For example, "Actor A" must be initialized, and "Actor A" and "Actor B" must interact successfully for the connection to be established.

![](https://www.plantuml.com/plantuml/img/RSz12W8n343HVKyHkjOBp4AcwiPZn3HgiAP39wFWwQqeP3EhG7Xyad4sRBOyIfZw5989rI0cijeWnRNjFeR65wXFRWvXW9EA2HPveqdL5RFnGkU7gihqfS3zY7EH-SOvU76xSpySqH14iz2lxYLdwS-GxbfVXVELmyYQ-tSV)

### Executions

When an actor takes a significant amount of time to complete an action and is unresponsive during that time, it is represented as a thin rectangle on the lifeline in the sequence diagram.

An _execution_ represents a period during which an actor:

- Executes an action or behavior
- Sends a request and waits for completion

An example is given in the figure below.

![](https://www.plantuml.com/plantuml/img/RP112W8n34NtEKNetWkua1aBNi0Jb3GoqTfYcuvuUpiWCnPcbOIyJ_xFKDHbhSy0PYHD6KU3XnF-XhCveaqnCYbcVbKk2evKPgUyCR1sVrBRf5UccaK_I1FJ0t9CJPPcpXi4Q9SToWtmFOboaABiWQF71OULx6rTd0JsMFeBUqR6BiEz5V2y4s9e9ziVlW00)

### Uncertainties

When an event's receipt is uncertain, such as in wireless transmission, a dotted open arrow is used. If an event is sent but not received, it is represented with an 'x' instead of an arrowhead, as illustrated in the figures below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBZWyWgwkdPs20a5AmNd_BoyrEGIXQBKmjBKuX8kXzIy5A3n0000)

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBZWyWgwkdOL20a5AmNd_BoyrEGIXQBKmjBKuX8kXzIy5A3n0000)

### Repetitions

To indicate repeated parts of a scenario, a loop can be used, as shown in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBXoylCBkE2A57Jj478ALWe1cNdfN0W1XAmZM3OVAAkdfwTWgb2ScfoVbrAi0EHnfUQbS3a02Q1J0000)

## Functional Decomposition

### Components

Functional decomposition involves grouping related functions into _components_. Components can either be _functional components_ or _asynchronous interface handlers_, with the latter managing asynchronous interactions.

### Component Diagram

Once functional decomposition is complete, the interactions among components are depicted in a _component diagram_. This diagram shows which components need to communicate to achieve specific functionality. The initial component diagram is illustrated below.

![](https://www.plantuml.com/plantuml/img/NSz12W8n30NGVKuHkfy3k11csYAuzWIXHXbiqw6D2nNlRjrewIRmUVylCbUbelSKeTvMsQXGmf3JbeL5J_g8Z8M3abmZTvKN81v5kLmey26twlfatECEtW2_FIv85OtxPzEX7T3sw0PqFVe1lG5OS4BxlHCwS2ruW9dbt5xx0000)

### Interfaces

_Interfaces_ define how components communicate, including the messages and parameters exchanged. Interfaces should meet these requirements:

- Facilitate message exchange between components
- Handle connections to the messaging system
- Fully isolate components from their environment
- Be specific to a functionality or responsibility

Multiple interfaces may be used between two components to separate different purposes.

## State Machine

The behavior of each component can be defined using a state machine. The _state machine_ specifies the possible states, transitions, and actions for each component. This approach provides a clear, formalized understanding of component behavior.

### State Transition Table

The _state transition table_ lists all possible transitions a component can make. It includes the current state, the interface and message received, the next state, and any actions taken during the transition.

### State Diagram

A _state diagram_ provides a graphical representation of the state transitions. It shows the states, decision points, and the paths between them.

Below is an example state diagram.

![](https://www.plantuml.com/plantuml/img/XPBRIWD138RlynHXBuK-W7lH0nN1co9jGKL4XfrWWvs3CrdhWRwxqtMxs-w0NYRvylz_Xfb6KK5giuKDQq8oNZvHKwm2Us5d0U8hMw-2CZWBmUtm6n2ldPKLVn5UmZuHeeImSiQZYbWidAOYOSV1JBzJ48viNzhEt5bBbMGDqiUT-UtpRiD2MuwfsyZUzVMa07YyUCBHQ7oc3YV5S8WbhjcGgoN71cubBjSVPPjfIM_faW5nsEbkIFm3nGofVHPWuP9Il_T7mbDgt3zF4-s-xEeRgw4Vvdg7EwuuLh_8V_JGlONEtPk7wSptC2Mhq_Vv0G00)

The initial state is marked with a black dot, indicating the state that the component starts in when initialized.

