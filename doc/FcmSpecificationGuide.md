# Functional Components Method Specification Guide

# Introduction

The Functional Components Method can be used to specify the software of a device.

This document gives some guidelines to document this specification.

# Use cases

Based on the required functionality, the system has to achieve certain objectives.

Examples of such system objectives are:

* Connection establishment
* File storage
* Connection monitoring
* Sensor readout
* etc.

To achieve a certain objective, some actors are involved. The objective and the list of involved actors define the use-case.

To achieve a certain objective, some actors are involved. Also, some use cases have a dependency to other use cases, which means that the corresponding objective can only be successfully achieved when the objectives of other use cases are achieved.

To define which actors are involved in which use-cases and the dependencies between these use cases a use-case diagram is used. 

An example is given in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuR9oJ2x9Br9mj5BGrRLJq0WjJbL8JWGIZfeufPpXKawuTM58EU6MmcEW2rpA6RU1pYJe0jg5x5nCCnrIyrA00GC0)


# Scenarios
_As described, [use cases](#use-cases) indicate which actors need to cooperate to achieve the objective identified by the use case. However, the use case does not describe how this cooperation is done. This is done by scenarios which are described in this chapter._


## Definition
Scenarios define the sequence of events and actions required to achieve the objective identified by the use case.

For a use case there can be several alternatives to achieve the objective. Also, it must be specified what needs to happen when some required events do not occur or actions fail and the objective cannot be achieved. This makes that a use case has a number of associated scenarios.

Also, a scenario has a _precondition_ which defines the situation at the moment the scenario is started.

To describe the cooperation between the involved actors, the scenario describes which events the actors indicate to each other. These events can be for example:

* finished a certain action,
* indication that something happened,
* request or command to do something,
* response to a request or command.

The sequence of event indications and required actions that occur in a scenario is described in a sequence diagram as discussed in the following sections.

## Simple sequence diagram

When an actor can reply immediately and no history is built up, i.e. no changes to the actor are made, the sequence diagram is simple as illustrated in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuL9oJ2x9Br9mL5BGjLE0SfoKh1G2ClFIkU1y94b7a6G-KD8XDIy5Q080)

## Actions

When something happens inside the actor as a result of the event, i.e. when the actor has to perform certain actions, this can be indicated by a comment box in the diagram as illustrated in the figure below.

![](https://www.plantuml.com/plantuml/img/RSz12W8n343HVKyHkjOBp4AcwiPZn3HgiAP39wFWwQqeP3EhG7Xyad4sRBOyIfZw5989rI0cijeWnRNjFeR65wXFRWvXW9EA2HPveqdL5RFnGkU7gihqfS3zY7EH-SOvU76xSpySqH14iz2lxYLdwS-GxbfVXVELmyYQ-tSV)


## Referencing

The dependencies between the use cases were already defined in the use case diagram. However, for convenience a reference to another scenario can be given in the sequence diagram.

In the example in the following figure, “Actor A” needs to be successfully initialized and both “Actor A” and “Actor B” need to have interacted successfully in the scenario in which the connection was established.

![](https://www.plantuml.com/plantuml/img/RSz12W8n343HVKyHkjOBp4AcwiPZn3HgiAP39wFWwQqeP3EhG7Xyad4sRBOyIfZw5989rI0cijeWnRNjFeR65wXFRWvXW9EA2HPveqdL5RFnGkU7gihqfS3zY7EH-SOvU76xSpySqH14iz2lxYLdwS-GxbfVXVELmyYQ-tSV)

## Executions

When sometime during the sequence an actor is engaged in an action that could take some significant time and during which the actor is unresponsive to other events, this can be represented as a thin rectangle on the lifeline.

Execution is an interaction fragment which represents a period in the actor's lifetime when it is:

* executing a unit of behavior or action within the lifeline,
* sending a request to perform an action to another participant, and waiting for the event indication that the requested action finished.

The duration of an execution is represented by two events, the start event and the finish event. The execution itself is represented as a thin rectangle on the lifeline. An example is given in the figure below.

![](https://www.plantuml.com/plantuml/img/RP112W8n34NtEKNetWkua1aBNi0Jb3GoqTfYcuvuUpiWCnPcbOIyJ_xFKDHbhSy0PYHD6KU3XnF-XhCveaqnCYbcVbKk2evKPgUyCR1sVrBRf5UccaK_I1FJ0t9CJPPcpXi4Q9SToWtmFOboaABiWQF71OULx6rTd0JsMFeBUqR6BiEz5V2y4s9e9ziVlW00)


## Uncertainties

When it is uncertain whether or not an event indication will be received by the other actor, for example in case of a wireless transmission, this is indicated with a dotted open arrow as illustrated in the figure below.

![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBZWyWgwkdPs20a5AmNd_BoyrEGIXQBKmjBKuX8kXzIy5A3n0000)

When the reception is uncertain, then there should also be a scenario in which the event was indicated, but not received. This is indicated by replacing the arrow head by a ‘x’ as illustrated in the figure below.


![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBZWyWgwkdOL20a5AmNd_BoyrEGIXQBKmjBKuX8kXzIy5A3n0000)


## Repetitions

To indicate that part of the scenario is repeated, a loop indication can be used as illustrated in the figure below.


![](https://www.plantuml.com/plantuml/img/SoWkIImgAStDuIe0qfd9cGM9UIKAIex99Vb52evAs4ITbBXoylCBkE2A57Jj478ALWe1cNdfN0W1XAmZM3OVAAkdfwTWgb2ScfoVbrAi0EHnfUQbS3a02Q1J0000)


## External function calls

Besides communicating via messages, there will also be the possibility for a component to call external functions. To be able to indicate this explicitly in the specification, the following three types are defined:


* _Control_ \
Initiating an action immediately, outside the message queue, will return the result later.
* _Library_ \
Execute a function in a linked library. The return value of the function can be used.
* _Database_ \
Perform a query. This could take some time and could return a relatively large result table.

In the sequence chart each type is indicated with a different symbol as illustrated in the figure below. 

![](https://www.plantuml.com/plantuml/img/ROwn2W9134JxV4LABVm1gmFFGh2njydjfGZS9SjU5jo_DuWh25VDCBmPPjYSYjTrWHmgIJAfuyNMRChga4ozs1B9Ho4oyOPtcGgL1ZCvJRGnZby3lohQwTbhUCQRYWiv7ux_YFMjOAulJdKFwUl1F2gN5iJ0EiVjDm00)


## Timer

To ‘Control’ function call can be used to explicitly indicate the scenario involving a timer as shown in the example in the figure below.

![](https://www.plantuml.com/plantuml/img/dP4n3iCm24Ltdy9ZE-G26QfKdJetOnU46ImvEB99pb-SP42Lqg6JpUV11tke0aMMAJc7cQNavCSuKN5kLZrYd875F_PKKjtAOm6kI75jl0byTpCbllUlPaBXoS4var3CLlN-7lIKMAcH1ipTUnzBcKhIhdcHorLTD_NRxm3-Cplt-h6X1anux7WoEG8Z9TCm4A345OIEHn-8W_x51m00)


# Functional decomposition


## Components

The functional decomposition of the device basically means grouping related functionality into components. The result is a table of components where the name of the component is specified next to a clear list of responsibilities.

Note that there are two types of components: functional components and asynchronous interface handlers, where the latter shield the asynchronous parts of the device.


## Component diagram

When the functional decomposition is done it is defined which components need to be ‘connected’, i.e. which components need to work together to implement specific functionality of the device. This interconnectivity is indicated in the initial component diagram as illustrated in the figure below.

![](https://www.plantuml.com/plantuml/img/NSz12W8n30NGVKuHkfy3k11csYAuzWIXHXbiqw6D2nNlRjrewIRmUVylCbUbelSKeTvMsQXGmf3JbeL5J_g8Z8M3abmZTvKN81v5kLmey26twlfatECEtW2_FIv85OtxPzEX7T3sw0PqFVe1lG5OS4BxlHCwS2ruW9dbt5xx0000)

The next step is to define the actual interfaces between the components, i.e. define the messages and their parameters that can be exchanged between the components and the function interface for the handlers. How this interfacing can be added to the component diagram will be described next.

## Interface

When the functional decomposition is done and the initial component diagram is constructed, it is defined which components are connected. The next step is then to define the _interfaces_ between these components. These interfaces are named and indicated in the component diagram as illustrated in the figure below.


![](https://www.plantuml.com/plantuml/img/PP0z3i8m34PtdyBg14CMDWQa9ewGc1Wu08gA-H5jMgNf08YxOrAfL65A_9uZvtFPHnVYqDJGts--Sy4rMBLDrthsyH0VDMFWAZf_gNdMyWB4dOySpgxYRIkjjoVZ1jVm1e354hL1rsCnzHorAKQ-Ic8Vhnp-BUMlA76o6ITR04p_H9qq_UfY9aqcpLnI9cakRIQ_GpKg2G6IJmsTGYD48Yc1uoMHL1D878K4a0uBaaAXbVsKx4-oyWy0)

Note that two components can be connected via several interfaces to make a more clear distinction between the different purposes of the interfaces.

The interface defines the messages and their parameters and must comply with the following requirements:

* An interface is used by a component to exchange messages.
* The interface implements the connection to the messaging system.
* It completely shields the component from this environment.
* The interface should preferably be limited to a specific functionality or responsibility.

Each interface must be uniquely defined in the system by its name. However, to create independence between different interfaces, the messages of an interface and their message parameters do not have to be unique.


## Interfacing to a asynchronous interface handler

An asynchronous interface handler has two types of interfaces:

* _Function interface_ \
Functional components that have a reference to the handler can call the non-blocking functions of the handler. In the component diagram this is indicated by a dotted arrow from the functional component to the handler.
* _Message sending interface_ \
The handler can send messages (i.e. ‘events’) on this interface to the connected functional component, but not receive any. In the component diagram the unidirectionality of the interface is indicated by adding an arrow pointing to the functional component.

In the figure below an example is given of the possible interfaces between a handler and a functional component.

![](https://www.plantuml.com/plantuml/img/LP1DRi9034RtFeML5IoIVIL094EHMB222rY9MuqwyKGp3XAWthqcKQjqPxtddq_UPwMaOnyWVtaPA56FRUo7A2nwqNjWJDmgoMVWHSiJ44-Yd3wevMEqLlzWtE8RV0F0QerrWvInSA8flDziLovUsCtuegHZ_kNmTn3hUSeMfXW2fsBXtEJEKRp6V-8mYPDnJy5oc0I97UDyx9f8Sil-Dea6ozBIWGKiTrY_mAQgvgAqVE0cuG0EMEsm0TYpTFQW7m00)

## Interface multiplication

The interface of a component can be multiplied in case more than one component needs to be connected to the interface. An example is given in the figure below.


![](https://www.plantuml.com/plantuml/img/TP0_2y8m4CNt-nH79nqApUWWr4Lm4yH9H8xsb62JbEGSLFpkXbPZH1pVUp_U_Qc3a9UBwI2SjUt9ay76cTvPjhAHQyVekH6ofuupv0w8AoliZzJmqaLKtnZdE8C7m6IAsxubuKE55B0OnQxQ5rcayaZ52D9ONBH6Mnt4apYV-MloWVshTHH3wm-XSc8e_wh4iamR_aqKGBeYkk_PaFR_c16jsRRnYqy0)

This diagram can be simplified as shown in the figure below.

![](https://www.plantuml.com/plantuml/img/LOyz2y9044Jx_HLBAYskiRCGOYFO2M8b5ajkbSFSNjXRmG_otpqnHkq6vis3gREHsYMsaCz1EbAAsAJO9M6nhTrQHkN6I4uj_o0FG5oBiHwfuLKgQBWpBd0EFS2unwMFGK8s9KiAemXdrs8eYiaKTvqdOwICrJlk3rLfleuDQMPzqNysT6xObcQGk0_S8zGiljnw0W00)


# State machine
_The functional decomposition of the device into a number of functional components could also be formulated as the statement that the behavior of a device can be defined by a number of state machines. In this chapter the way in which the state machine can be specified is explained._

## Settings

The settings of the component are supplied at the instantiation of the component as these cannot be changed by the component itself.

The table listing the settings comprises the columns: name, type and description.

## State variables

Some attributes of the functional component are used inside state transition and evaluation functions. Those specific attributes are called state-variables as these can be technically seen as holding a kind of 'sub-state' of the component and influence the behavior of the component.

The table listing the state variables comprises the columns: name, type and description.


## States and choice-points

The first step in defining the state machine is listing the possible states and choice-points.

The state table specifies:

* _State name_ \
The state name gives a short descriptive indication as to what the functional component is doing in this state.
* _Description_ \
The description gives a more elaborate description of the activity in this state.

The choice-points table specifies:

* _Choice-point name_ \
The choice-point name is formulated as a short closed question.
* _Evaluation_ \
The evaluation gives a more elaborate description of which state variables need to be checked to answer the question.

## State-transition table

To specify all the transitions that can occur for the functional component, a state-transmit table must be specified. This table comprises of the following columns:

* _State_ \
The current state on which messages can be received. All messages for the state are grouped together in consecutive rows. The entry can also be a choice-point in which case there will be two transitions corresponding to the logical messages “Yes” and “No”.
* _Interface_ \
The interface on which the message is received. Messages on the same interface are grouped together in consecutive rows.
* _Message_ \
The message on the specified interface that causes the transition.
* _Next state_ \
The destination of the transition. This can also be the same state, or a choice-point.
* _Actions_ \
The actions that are performed when transitioning to the next state.

As a simple example consider the following system:


> _The system comprises a wireless system in which a node wants to connect to a host._

Suppose that the node is decomposed as given in the following figure.


![](https://www.plantuml.com/plantuml/img/LL3B2i903BpdAu8dFTIxe58GH2yYjSTUmZPAwJPRTbVnWV_ksfO-3Y73P38P4ZjFrjyh3Quif2PB5IfJrKPO_CM_DADbvKbkcaUI3o0UnREzakAz2TBYpRZ15Nm1OBx097t-svbKoTu1z9wOMXAdk7Ym7R6xu66DrcoH7CvE9kTC1d868872XFB2D8e6PN8WoTlvo1UZvdweW0QJaApTZ_fSNSTeEydLwTRBAHkpvE5LFm00)

Now suppose that the behavior can be described as follows:

> _The node starts regularly advertising its presence after it is initialized. The host will request to connect when it receives this advertisement. When the node receives this request, it first checks whether the host is allowed to connect. If this is the case, it acknowledges it after which the host will respond with an indication that it is now connected . The node will respond to this indication. To monitor the connection on both sides, the host re-sends this connected indication in regular intervals. When the connection is closed or lost, node starts advertising again._

For the “Node Controller”, this narrative specification could be translated into a state-transition table which is given in the following table.

<table>
  <tr>
   <td>State
   </td>
   <td>Interface
   </td>
   <td>Message
   </td>
   <td>Next state
   </td>
   <td>Actions
   </td>
  </tr>
  <tr>
   <td rowspan="2" >Advertising
   </td>
   <td><code>RTx</code>
   </td>
   <td><code>ConnectReq</code>
   </td>
   <td>Whitelisted?
   </td>
   <td>
<ul>

<li>Cancel advertising timer.

<li>Store <code>clientId</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>Timer</code>
   </td>
   <td><code>Timeout</code>
   </td>
   <td>Advertising
   </td>
   <td>
<ul>

<li>Call <code>advertise()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td rowspan="2" >Whitelisted?
   </td>
   <td rowspan="2" ><code>Logical</code>
   </td>
   <td><code>Yes</code>
   </td>
   <td>Connecting
   </td>
   <td>
<ul>

<li>Send <code>RTx:ConnectAck</code>.

<li>Start the connection timer.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>No</code>
   </td>
   <td>Advertising
   </td>
   <td>
<ul>

<li>Send <code>RTx:ConnectRej</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td rowspan="2" >Connecting
   </td>
   <td><code>Timer</code>
   </td>
   <td><code>Timeout</code>
   </td>
   <td>Advertising
   </td>
   <td>
<ul>

<li>Call <code>advertise()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>RTx</code>
   </td>
   <td><code>ConnectedInd</code>
   </td>
   <td>Connected
   </td>
   <td>
<ul>

<li>Call <code>connected()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td rowspan="3" >Connected
   </td>
   <td rowspan="2" ><code>RTx</code>
   </td>
   <td><code>ConnectedInd</code>
   </td>
   <td>Connected
   </td>
   <td>
<ul>

<li>Cancel the connection timer.

<li>Call <code>connected()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>DisconnectInd</code>
   </td>
   <td>Advertising
   </td>
   <td>
<ul>

<li>Call <code>advertise()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>Timer</code>
   </td>
   <td><code>Timeout</code>
   </td>
   <td>Advertising
   </td>
   <td>
<ul>

<li>Call <code>advertise()</code>.
</li>
</ul>
   </td>
  </tr>
</table>


Note that the `clientId` is a state variable.

As can be seen, a reference is made to the `advertise()` function in this function the following actions are performed:

* Send `RTx:AdvertiseInd`.
* Start the advertising timer.

Similarly, the `connected()` function performs the following actions:

* Send `RTx:ConnectedInd`.
* Start the connection timer.

In this way, a group of actions that is repeated for several transitions can be grouped into a single function to prevent copying the actions (and eventually code).


## State diagram

The state diagram graphically shows the states, choice-points and transitions.

As an example, consider the figure below containing the state diagram corresponding to the state transitions table given in the previous section.


![](https://www.plantuml.com/plantuml/img/XPBRIWD138RlynHXBuK-W7lH0nN1co9jGKL4XfrWWvs3CrdhWRwxqtMxs-w0NYRvylz_Xfb6KK5giuKDQq8oNZvHKwm2Us5d0U8hMw-2CZWBmUtm6n2ldPKLVn5UmZuHeeImSiQZYbWidAOYOSV1JBzJ48viNzhEt5bBbMGDqiUT-UtpRiD2MuwfsyZUzVMa07YyUCBHQ7oc3YV5S8WbhjcGgoN71cubBjSVPPjfIM_faW5nsEbkIFm3nGofVHPWuP9Il_T7mbDgt3zF4-s-xEeRgw4Vvdg7EwuuLh_8V_JGlONEtPk7wSptC2Mhq_Vv0G00)

The first state is defined by the initialization point, indicated by the black dot. The state is set when the component is initialized.

Note that for the “Timeout”, “Yes” and “No” messages, it is not necessary to specify the interface.
