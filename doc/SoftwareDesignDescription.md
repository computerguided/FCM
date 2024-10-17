
# Software Design Description layout
_This document describes the layout of the Software Design Description according to the Functional Components Method._

## Objective
The Software Design Description (SDD) captures the way the use cases are implemented in the behavior of the device. When there is only one device, the SDD comprises the following sections: device, hardware interface and components. The contents of each section is given in the following.

## Device
On the highest level, the SDD starts with the Device description. This section contains:
* **Component diagram**\
The component diagram is the result of the functional decomposition. It is a graphical representation of the components and their interfaces. Next to the diagram, for each component the _role_ is specified. Typically, the components are separated into _functional components_ and _asynchronous interface handlers_.
* **Interfaces**\
In an overview table the description is given for each interface. Next, for each interface the messages and their description are given. For each message that has parameters, a separate table specifies the type and description of each parameter.
* **Device configuration**\
Settings and constants that are used by the components.
* **Device phases**\
Abstract phases that the device goes through from an outside view.
* **Functionality**\
The functionality of the device is described by first specifying the _actors_ and then listing the _use cases_ with their objective and actors involved. Each use case is described in a separate section containing a detailed description of the use case and a list of scenarios. Then for each scenario the preconditions, sequence of messages and actions, and the postconditions are specified.

## Hardware interfaces
The FCM defines asynchronous interfacing to hardware. The used hardware interfaces and the associated behavior and used functionality is described.

## Components
Each component is described from the perspective of the component. This description contains the following:
* **Context**\
The component operates in a certain context which is basically a subsection of the component diagram comprising the component and its direct neighbors.
* **Roles**\
Within the given context, the roles of the component itself and those of the connected components are described.
* **Behavior**\
The textual description of the behavior of the component.
* **Settings**\
The settings of the component which are used to configure the component at runtime when it is created in the device and might be changed in different configurations of the device.
* **Constants**\
Constants that are used in the component and that (normally) will not change and are only relevant within the context of the component.
* **State variables**\
To support the implemented functions and the state machine, certain local variables, or _state variables_ are defined.
* **States and choice-points**\
For each state and choice-point a description is given.
* **State diagram**\
A graphical representation of which messages create a state transition.
* **State transition table**\
The state transition table is a textual representation of the state diagram in a sense that it defines the next state given the reception of a specific message in a specific state. In addition it defines for each transition the actions to perform, functions to call or message to sent.
* **Functions**\
Detailed design of functions which are called in the state transitions, e.g. to capture a number of repeating actions that occur in more than one transaction.
