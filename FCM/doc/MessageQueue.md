# Message Queue
****
_Messages exchanged between components are stored first in the message queue. The message queue is a first-in, first-out (FIFO) data structure. Messages are stored in the queue until they are processed by the receiving component._
****

## Description

The message queue is a crucial component in the system, responsible for storing and managing messages exchanged between different components. It operates on a first-in, first-out (FIFO) basis, ensuring that messages are processed in the order they are received.

The message queue acts as a temporary storage for messages until they are processed by the receiving component.

## Submessage Queue

Handlers can have a sub-message queue that is used to store messages that are sent by the handler. At the beginning of the processing of the message queue, the sub-message queue is emptied and the messages are added to the message queue as described further in ["Device - run loop"](Device.md#run-loop).

