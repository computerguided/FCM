# Device
----
_The system can be specified by identifying the actors, the use cases and the scenarios. Some of these actors are devices._

----
## Definition

Some of the actors of the system can be denoted as _devices_. On an abstract level these devices are characterized as _actors_ that can have a certain _state_ (or combination of states) and demonstrate deterministic _behavior_.

More practically, a device is defined here as follows:
* The functionality and behavior of the device is implemented in a modular way by defining a number of functional components.
* These components are specified to be interconnected in a component diagram.
* Via these connections the components can transmit messages.
* There is one shared message queue.
* No messages are lost.
* There is shared memory between these components.

## Functional decomposition
A device can be in a specific state for one use case but at the same time it could also be in another state in another use case. This could result in complex implementations if not some decomposition is performed. 

This decomposition can be done by deriving functional _components_ from the use cases and scenarios.

These components have _interfaces_ by which they are connected to one or more other components. Via these interfaces the components can send and receive messages to the connected components. As such the components can be seen as message handlers that implement a number of scenarios.

The implementation of the message handling inside a component is done by defining a _state-machine_.

Some components can be associated with functionality that includes interaction with hardware. These components have interface-ports on which hardware can 'inject' asynchronous messages to the components. This can be done via a polling-loop or via an ISR.

The components in turn can call (non-blocking) functions from the hardware API.

## Construction

1. Initialize the asynchronous message queues.
1. Construct all handlers.
1. Create all components.
1. Connect all interfaces.
1. Connect all handlers to components



## Event loop






