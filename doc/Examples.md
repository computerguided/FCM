# FCM Best Practices and Examples
_This is a living document containing a selection of the best practices and examples for the Functional Components Method._

## Add a state transition

There are two methods available to add a state transition:

* Using the `addTransitionFunction()` method
* Using the `FCM_ADD_TRANSITION` macro

In the following sections, the two methods are described in detail.

### Using the `addTransitionFunction()` template method

The `addTransitionFunction()` is an inline template method defined as follows:

```cpp
template<typename MessageType, typename Action>
inline void addTransitionFunction(const std::string& state, const std::string& nextState, Action action)
```

As an example on how to use this method, consider a component that is waiting for a connection request ("ConnectReq" on the "Transceiving" interface) from a server. When the connection request is received, the component can be transitioned to the next state.

```cpp
addTransitionFunction<Transceiving::ConnectReq>("Advertising", "Correct server?",
    [this](const auto& message)
    {
        cancelTimeout(timerId);
        serverId = message.serverId;
        connectionId = message.connectionId;
    }
);
```

In the example, the `NEXT_STATE` is specified as “Correct server?”. The question mark indicates that the next state is one of the choice-points, which will be described in the section ["Add a choice-point"](#add-a-choice-point).

Note that when no action is needed, the code can be simplified by using a lambda function that does nothing.

```cpp
addTransitionFunction<Transceiving::ConnectReq>("Advertising", "Correct server?",
    [](const auto& message){}
);
```

### Using the `FCM_ADD_TRANSITION` macro

The second method that is available is using the `FCM_ADD_TRANSITION` macro.

```cpp
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)
```

The macro takes five arguments: the current state, the interface, the message, the next state and the action. The action is a lambda function that is executed when the state transition occurs. Inside the action, the message can be used to extract data and to send messages to other components or handlers. This is possible because the message is automatically cast to the correct type.

By specifying the `ACTION` as the last argument, the macro can be conveniently used. Consider the earlier example of a regularly advertising client that is waiting for a connection request from a server:

```cpp
FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Correct server?",
{
    cancelTimeout(timerId);
    serverId = message.serverId;
    connectionId = message.connectionId;
});
```

Note that the `message` is defined and available inside the macro and is automatically cast to the correct type.

When no action is needed, the code can be simplified.

```cpp
FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Correct server?",
{
    // NOP
});
```

### Which method to use?

The `FCM_ADD_TRANSITION` macro is more compact and convenient to use while coding, especially when the action is a simple and only contains a few lines of code. However, when the action becomes more complex, the method suffers from the standard problem with macros as it becomes difficult to debug using breakpoints because the macro is not expanded inline and the program will halt on the wrong line. A possible practice is to define the action as a separate method and then call this method inside the macro.

Another option is to use the `addTransitionFunction()` method, which is recommended when the action becomes more complex. In that case, breakpoints can be used in the action to debug the code.

## Accessing the last received message

In several circumstances it might be useful to store or access the parameters of the last received message. For example to use the value of one or more of its parameters in the evaluation of a choice-point.

One way of doing this is to access the message parameters and copy the values in state variables.
However, since this is such a common mechanism, the `lastReceivedMessage` attribute is defined for a Functional Component in which the last received message is stored.

Since this attribute is stored as a `FcmMessage` base class, it needs to be first cast to the proper type. To do this, the `castLastReceivedMessage()` template method can be used.

An example of the use is given in the code below where the lastReceivedMessage is cast to the `Commands::DataReq` message.

```cpp
addChoicePoint("Correct Id?", [this]()
{
    auto dataReq = castLastReceivedMessage<Commands::DataReq>();
    return dataReq->id == myId;
});
```

If the supplied interface and message type are incorrect, then a runtime error will be thrown, for example:

`Last received message cast to invalid message type "Commands:DataReq"!`

## Replacing if-statements

Consider an example in which a functional component is waiting for a `Commands:DataReq` message in the state “Awaiting Request”. When this message is received, the functional component needs to transition to the “Request Received” state. The request contains an id which needs to match the id of the component as stored in its `myId` state variable. When this is the case, the `Command:DataRsp` is sent. However, when the id is not correct, the `Command:DataRej` must be sent with the message parameter set to “Request with wrong id”. The corresponding state machine could be something like shown in the following figure.

![](https://www.plantuml.com/plantuml/img/JOz12i9034NtEKNeKl00TY4jYk1ETAakGYTei3DJ9wb5nRixbYxyk_1V7lnAZPBrleMhE4RsdJtHiJP9EfCO0FGceQD47kkKue1lGDp4O0TvCIxW0pbPOenRrpAIOb4F92RXSWfxllUiLap4BiM6LNCrSbFxmngM1xi2u3WxutmvkW16Oxx-_afSHUyfE2tNP9GL01K7boTy0G00)

The transition code for this state machine could be as given below.

```cpp
FCM_ADD_TRANSITION("Awaiting Request", Commands, DataReq, "Request Received",
    if (message->id == myId)
    {
        FCM_PREPARE_MESSAGE(dataRsp, Commands, DataRsp);
        FCM_SEND_MESSAGE(dataRsp);
    }
    else
    {
        FCM_PREPARE_MESSAGE(dataRej, Commands, DataRej);
        dataRej->message = "Request with wrong id"
        FCM_SEND_MESSAGE(dataRej);
    }
);
```

While this is technically correctly implemented, it does obscure what’s actually happening. A better way of doing this is to make the condition-statement explicitly visible by using a choice-point as shown in the figure below.

![](https://www.plantuml.com/plantuml/img/VP3F2i8m3CRlUOgme-03k8D_60YxoD2JE96o1YskxMoYGyLtjniJTZAdXE-NV4cMB2h8tTPmCPgGR2DFrCHLC8qOxm3uQboZWhAu2i6t-0R4jNUoCo_26NmWHXmYXBck2HLZicgD67SktPPkTs991g08lYBcAFNOe7POHUP1ElbDoefJvaEWIdfmo4kNwqLa3fCZJkUz7K1l6gjHIugPjrOvpUckkoxw08pqI8ysIN5F_4_UU80bEHsVz0K0)

The corresponding transition code will then be as follows.

```cpp
FCM_ADD_TRANSITION("Awaiting Request", Commands, DataReq, "Correct Id?",
    // NOP
);
FCM_ADD_TRANSITION("Correct Id?", Logical, Yes, "Received Request",
    FCM_PREPARE_MESSAGE(dataRsp, Commands, DataRsp);
    FCM_SEND_MESSAGE(dataRsp);
);
FCM_ADD_TRANSITION("Correct Id?", Logical, No, "Received Request",
    FCM_PREPARE_MESSAGE(dataRej, Commands, DataRej);
    dataRej->message = "WARNING: request with wrong id"
    FCM_SEND_MESSAGE(dataRej);
);
```

The choice-point that needs to be defined is done as given below.

```cpp
FCM_ADD_CHOICE_POINT("Correct Id?",
    const auto& dataReq = FCM_CAST_MESSAGE(lastReceivedMessage);
    return dataReq->id == myId;
);
```

## Resending message

Consider an example of a state machine for a “Database Manager” which resides in a certain state “Idle” and can receive a message to add something to the database (`Database:AddInd`) or remove something from it (`Database:RemoveId`). These messages contain the parameter `id` that needs to be checked. If it is not correct, the message can be ignored. In case the `id` is correct however, the indication can be forwarded to the database. When the database is ready with the action it will respond with `Database:ReadyInd` which contains the number of elements currently in the database. When something was added, then the state can return to “Idle”, but when something was removed, it must first be checked whether the database is empty.

This behavior can be implemented in the state machine shown in the figure below.

![](https://www.plantuml.com/plantuml/img/TPB1JiCm38RlUGfhHwG-03kCJGEaNT0qJcWW56O34MrI9TxGDl7kk4dAsdLSsjh_vzz_eau3Qy_RgbIV1WcegdaFI67ZJSt6MQN2bx6rzhg2cVVk6uuAuDvPVZG7Wclr8mHh9bXHvNR6VkHoXdWg5bZIEPRh-V9rxhod3KU5G0SOfSzdcnKmU3CQyB8XmlB-ZxoBnscWMytwJGVnZ3s1b5fVlK0nIU5KyuHYMqjZ46aCxVWuTHOMyy3aB507bRBXrC1EXjfR4Fw9mWKavnBzmNKjcXsYfTiTJ4UrF_gtgKXPqvNqJgHntmPDQb7IEvziyewizTnpZixE2xvJiYX_s2y0)

Although the check  of the messages is the same, two choice-points ("Add Correct Id?" and "Remove Correct Id?") are required because the processing is different. While this is technically not a problem, it does make the state machine more complex, especially when there would be even more messages that need to be handled in a similar way (e.g. `Database:ReadInd`, `Database:ChangeInd`, etc.).

For this purpose, FCM introduces two mechanisms:

* `lastReceivedMessage` \
  This is an attribute of a Functional Component in which the last received message is stored.


* `resendLastReceivedMessage()` \
  This is a method of a Functional Component that resends the last received message as stored in `lastReceivedMessage`.

The `resendLastReceivedMessage()` uses the `resendMessage()` method of the message queue to resend the last received message.

```cpp
messageQueue.resendMessage(lastReceivedMessage);
```

This replaces the message at the top of the message queue, whereby it will be _immediately_ received again by the Functional Component before any other incoming messages are processed.

Using these mechanisms, the state machine can be simplified as shown in the figure below.

![](https://www.plantuml.com/plantuml/img/RPB1JiCm38RlVWfhHwG-03l0fW7IBcY24zeGCes1Y3Qf4c_JXdXtqZPjaubBbFZ__lXtb8KNShAlA_ZIYf7hHawes9TEDwAj0V3VsZJagCQbS_Q8Fu3uO8qywpFZDVm6GaWODywMxBqsdx6mLXN7xHFNzj0gyRpQlAsiSrmAaiVPiDVgTZOHzrsMLhyZeNVol3DTBK00swjNB6xw9j2kM8HZSl4S1zjygTJQg9qP2rqU3ZM0n17yqn31_y8-asExe3nQW0IzD5uqpj1z4-ZXFL8soJJAHSKVqe_LlqR6apeD8_HgqV5ZofW_QbduBD8a_NFvWeqAt-CF)

In this state machine, the “Yes” transition for the “Correct id?” choice-point would be like given below.

```cpp
FCM_ADD_TRANSITION("Correct id?", Logical, Yes, "Processing",
    resendLastReceivedMessage();
);
```

By doing this, the received `Database:AddInd` or `Database:RemoveInd` messages will be received again in the “Processing” state and can be handled because at that point it is ensured that the `id` parameter of the message is correct.

Note that the ‘messages’ on the “Logical” interface (i.e. `Logical:Yes` and `Logical:No`) are not stored in the `lastReceivedMessage`.

## Multiple state transition

### Multiple-states messages

Consider a simple example of a database connection. When the connection is established (e.g. by a handler), the `Database:ConnectedInd` is received. The connection can be used to add and remove data. This could for example be initiated by another connected functional component that sends the `Command:AddInd` and `Command:RemoveInd` respectively, where it is assumed that the corresponding database action can take considerable time. The corresponding state diagram could be as shown below.

![](https://www.plantuml.com/plantuml/img/TL71IWD13BtdAqAFmdw0UwWjLQ4NaNeI5ObDqC5EPfc9IXN_tPdjx7OHUXcIzrxUIvX5CehsuNVmvbXGV6TxP4dRw3fpGG7IkzEE8dbSnXY-y0SGhuFQdViMF8VVh30omIMpqzVQh7adjLm5LTcQSEqtuiDdKGxyxVDLdqe9PvTaz4997hN7BcO03sTFsCmFZb3UqescsE8mquxWMXdWA6dom92QzQlWFIctFIH5Ev7EwnqJNGPwmqeKjt7dIVf6YFU7v85j_crwB-aqUHC0nYsRKpSUKnQYdF_j3m00)

Now suppose that the connected database sends regular keep-alive messages to ensure that the connection is still available. The client is then required to reply with a response. If this must be added to the behavior, the corresponding transition must be added to each state as shown below as it can be received in any state.

![](https://www.plantuml.com/plantuml/img/TPB1IWGn38RlUOgmHs4Um3de3gl28eYC9r6HkerQdAQrxQwiuhlRpap7kZANqYHV_kGlNVeWNDZg3bwL92Hjmnub-OrJDYZ30FvDiHLEQ6oSCn_u1OWNXiED-YGyXkz811486oaLlup1MdOqNbU6cJQ1v1YtfCqkaRb-_NZUJnKU5sSYY2VXwPxxtEa2uExe0QkJGH7IcQ9956lCFVMKNBC4-4Mgs92dHdvbj1Oiwpv5YItGhOtmOAL0O-A0czlXaiWsdTfHI-y0er3LqND87TbgqT2IaFiadjbo_d_PmLhMELpZblnyySe0JCymQs3QvmzQFD_CJajY6R_H3m00)

While this is technically correct, it means that the `Database:KeepAliveInd` transition needs to be handled in each state, making both the state diagram and state transition table less straightforward. To handle these types of situations, there are two options that are discussed in the following sections.

### Adding multiple-states transition

On the coding side, a practical way of adding a multiple-states transition is using the `FCM_ADD_MULTIPLE_STATES_TRANSITION` macro in combination with the concept of the ‘history-state’.

```cpp
#define FCM_ADD_MULTIPLE_STATES_TRANSITION(STATES, INTERFACE, MESSAGE, NEXT_STATE, ACTION)
```

The code for the example will then be as given below, where the history-state is indicated by “H”.

```cpp
FCM_ADD_MULTIPLE_STATES_TRANSITION(({"Connected", "Removing", "Adding"}), Database, KeepAlive, "H",
    databaseInterface->keepAlive();
);
```
**_Note the use of the parentheses and curly brackets to define the list of states!_**

The result of this is that a transition is added for each one of the listed states. By defining the next-state as “H”, the state machine returns back to the previous state (i.e. creating the self-transition).

While this reduces the amount of code that needs to be added, the state diagram and state-transition table would typically not be changed as the transition still needs to be added for each state. To solve this problem, the concept of a ‘wildcard’ state can be used as described in the next section.

### Wildcard state to history

The other way of handling the multiple-states transition is to use the concept of a ‘wildcard’ state – or ‘any-state’ – which is indicated by a “*” and typically works together with the concept of the history-state.

Considering the example, the diagram can be changed as shown below.

![](https://www.plantuml.com/plantuml/img/TP7TIWCn48Nlvoc6NXRs0ToBjikgM8GYzKfgaTWPQd0pMPAehEAxc-m_Gc_2pfalSyviDZue5pvC1M-Q6DdKeK5YVtAw3jeAW7_NKYkd31REsI_y0SHRA-5HVpDUmMyaWWgC1P6MSrzigUB-ixGYV0fCVRrdOpydif06bSV5SZ4CAXzURjeOIRvMGRqgpy_IQkj87PP7p5QT1QGpLQCBvZZSoKTnAmGm8LcyCCI8V6cDKK9vAt5YP-YgNsp6HQ4Ts3VIj37pp7tFYfhEUUXc_v8-iR_St5c0CMLsQSV99Vt9X1tkZdFWdhakAjqj1XiMYg_z1m00)

In code this can be added as follows.

```cpp
FCM_ADD_TRANSITION("*", Database, KeepAliveInd, "H",
    databaseInterface->keepAlive();
);
```

In the state-transition table this can be added as follows.

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
   <td>*
   </td>
   <td><code>Commands</code>
   </td>
   <td><code>KeepAliveInd</code>
   </td>
   <td>H
   </td>
   <td>
<ul>

<li>Call <code>backendInterface:keepAlive()</code>.
</li>
</ul>
   </td>
  </tr>
</table>

### Using wildcard state as a bypass

Note that the transition coming out of an any-state does not always have to go to the history-state. As an example, assume that to allow for a graceful disconnection, the `Database:DisconnectInd` can be received and that this would mean that the state can return back to “Idle”. This can then be changed in the state diagram as shown below.

![](https://www.plantuml.com/plantuml/img/TPBTIiGm48NlynJ3NYxq0Up5kgMhk0WYwvMiYyJEeC5cKfAeLF7TJTBq1s5lIkRCbpbdGhVEI-i_T2lU51AGxdmFIAwnglFAi13kNN4dhTHGMMk-u4S0N1ls3-gRu4By1i9BJr0XAdxDnHvRoiVQC5FZ2NDz86q-PxBY7gI3rNer3ghldwzIZ2Zlf9Slqj4J9-qoKCVr2OhDO27YDrQJ2vGmtYadSSyen8mKuS8O8_2rqLeobacYo2xGJLvimGKX3SoDE6tAl70_aCH-S1wxnR-aZ-JED--C45FAujoEiqjyoK0TRqxB_YrHLxLgs2iX-UsMq4wvPXYU9cs9CVmIVm00)

In the state transition table this can be added as follows.

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
   <td rowspan="2" >*
   </td>
   <td rowspan="2" ><code>Commands</code>
   </td>
   <td><code>KeepAliveInd</code>
   </td>
   <td>H
   </td>
   <td>
<ul>

<li>Call <code>backendInterface:keepAlive()</code>.
</li>
</ul>
   </td>
  </tr>
  <tr>
   <td><code>DisconnectInd</code>
   </td>
   <td>Idle
   </td>
   <td>
<ul>

<li>Cleanup data administration.
</li>
</ul>
   </td>
  </tr>
</table>

Note that it is also possible to add choice-points after the any-state as these are not ‘real’ states.

## Creating loops

Consider an example in which a server has a component that takes care of updating connected clients when the database is updated which is indicated by receiving the message `Database:UpdatedInd`. The action that is required is then to loop through the list of connected clients and send a `Client:UpdateReq` to each one and await the response.

At first, it seems like this requires a for-loop in the transition code. However, a better way of doing this is to implement the loop in the state machine.

To implement this, two state-variables are defined.

```cpp
std::vector<ConnectedClient> connectedClients;
uint clientIndex;
```

The state diagram that could be used to implement this could then be as given below.

![](https://www.plantuml.com/plantuml/img/LP1D2i9038NtEKLmANW0kz1AHN1JH74XAXATe8Fjp30JyG_lRbixefkGbtpl4P868I-Nie2JLenSEhcZud3msecs1Y2SjN7agSIHz_QAJq2SM2CB_M3imwiYX8HngWfkswLJ9DeSaG9sejYOhD1if1CDsMoNyqqQg6uYCAo8TNUBlS4d4-fQgsX8S4n2UmgSDE6ifaO1n7Nhgx3S_Ww_LoMuuW3mrN_Hdnj2cpmF3Y1beweFlG40)

The possible code for the transitions is given below.

```cpp
addTransitionFunction<Database::UpdatedInd>("Idle", "Next Client?", [this](const auto& message)
{
    clientIndex = 0;
});
addTransitionFunction<Logical::Yes>("Next Client?", "Updating", [this](const auto& message)
{
    auto databaseUpdatedInd = castLastReceivedMessage<Database::UpdatedInd>();
    prepareMessage<Clients::UpdateReq>(clientUpdateReq);
    clientUpdateReq->data = databaseUpdatedInd->data;
    sendMessage(clientUpdateReq, connectedClients[clientIndex]);
});
addTransitionFunction<Logical::No>("Next Client?", "Idle", [this](const auto& message)
{
    databaseInterface.updated();
});
addTransitionFunction<Clients::UpdatedInd>("Updating", "Next Client?", [this](const auto& message)
{
    clientIndex++;
});
```

The choice-point that needs to be defined is as given below.

```cpp
addChoicePoint("Next client?", [this]()
{
    return clientIndex < connectedClients.size();
});
```

## Interface multiplication

The interface of a component can be multiplied in case more than one component needs to be connected to the interface. An example is given in the figure below.

![](https://www.plantuml.com/plantuml/img/PP2n2i8m48RtUugZamw5PdGGAeBW9eYJY1pjAS4qASavgFZkXbf3r37tVVn_xkeWvENQ6GWNRNlor67ZkjvPjhALcs7qt0ZPi-5CUG3YsWhx4pMySb7LTyOvpk09C9dYhcz9-5WX1IpUZtrrA3AaSgGYWbIBYxRJLWVn9Cvdymtvm7vCNHeTPqFgdw5oOmZ_YiGoVV0N63v9eWeWBLAcTaWRZFQd5A1csyPRlW00)

This diagram can be simplified as shown in the figure below.

![](https://www.plantuml.com/plantuml/img/LSyz2y9030RWFRyYTDAXYvk3K144Dw4uYKDeenpUvKekWX_qltjggRg5v7qVI9MCr2x1GpexwKWfO1D35uN5QhjvHkN6I4wUVo8FGDo8iHwfuNNCKNTdNE0SUe2nZyisE779b2mgZ13Ehbd8n6IAkwubOwI4nMVS7ufy-HfRqiJw3goyovlYrVoJiHoOV1YySkX1Zr2njFd39m00)

When in this example the “Administrator” component wants to send a message to a specific “Client”, the index must be specified as an argument in the `FCM_SEND_MESSAGE` macro as shown in the transition code example below.

```cpp
prepareMessage<Update::DataInd>(dataInd);
dataInd->data = pendingData;
sendMessage(dataInd, connectedClients[clientIndex]);
```

## Using the timer

Every functional component has a reference to the Timer Handler stored in its inherited `timerHandler` attribute via which it can set and cancel timers.

As an example of using a timer, consider a simple database connection monitor which receives a `Database:ConnectedInd` when the database server is connected. Once connected, a `Database:KeepAliveInd` is received at regular intervals of 5 seconds. When this doesn't happen for more than 10 seconds, the connection is assumed to be lost. In order to monitor this, a timer is started when the connection is established and canceled and then restarted when the `Database:KeepAliveInd` is received.

A possible state diagram can be as shown below.

![](https://www.plantuml.com/plantuml/img/TO_12i8m38RlUOgSXJs0EyY68WoFxYOUwXemkAQbpHGLtzsEoGJ1IqZoVnyXLL8JTN0zdDaIaWjwHqkfYnoKlG2a2qimqJYiO_GtV0BYrelk-K6uX5Scr2XXOtlwj6ilGfsI1JWiZbYifX36EavpZYLkZ9gJILJEoqQozqMAVyAEADGzN-bNcCvbj6L7Vb00g4XiVl8D0)

To implement the described behavior, first, to be able to store the timeout, a setting is defined.

```cpp
FcmTime connectionTimeoutMs{};
```

When a timer is created, a unique ID is created, which is required to cancel a running timer. Therefore, a state variable is defined to hold this ID.

```cpp
int connectionTimerId = -1;
```

Typically, the connection timeout will be set in the constructor. For the “Database Monitor” this is done as shown below.

```cpp
DatabaseMonitor::DatabaseMonitor(const std::string& nameParam,
const FcmSettings& settingsParam): FcmFunctionalComponent(nameParam, settingsParam)
{
    setSetting<FcmTime>("connectionTimeoutMs", connectionTimeoutMs);
}
```

With this set, the timer can be used as shown in the code below.

```cpp
addTransitionFunction<Database::ConnectedInd>("Idle", "Connected", [this](const auto& message)
{
    connectionTimer = setTimeout(connetionTimeoutMs);
});
addTransitionFunction<Database::KeepAliveInd>("Connected", "Connected", [this](const auto& message)
{
    cancelTimeout(connectionTimer);
    connectionTimer = setTimeout(connetionTimeoutMs);
});
addTransitionFunction<Timer::Timeout>("Connected", "Idle", [this](const auto& message)
{
    // NOP
});
```

# Handling switch statements

Consider an example in which a component sends a request on a specific interface – for example after it is connected to a remote server – and then receives a response with a `status` parameter indicating whether the request was “Accepted”, “Rejected” or “Pending”.

Depending on the value of the `status` parameter, the behavior of the component will differ. A possible state diagram can be as shown below.

![](https://www.plantuml.com/plantuml/img/PP51Qm8n48Nlyok6FHRy0Ts3LOI2boBRazGYOJEqQTr9J4Qa5V-xoPgmgrv2tfllJHva5aHvENGx-3QQa3edVwWfjDuuCPO1mgzXfxpgSEwzFU8945yjoxlv9to6SoH42U5IxoXVvqTbnF1NGy5P3eGgO5NC3HUxAdZRaXFIMQw8TUJkL9QBrRRW_TQ6zWSAik7YlbG3cREtO3OZ1nzFdpYPNfj3EfDwA5_ZmZ9J6vSlMGCy034qhbLZVgi93c0ycQJr0xQcS3kUZalN-6RxU35Ikg6VliXrUZSSacappAAC7ti1)

Note that the evaluation functions of the choice-points check the `status` parameter of the last message.

While this works, the disadvantage of this method is that if a certain value of the checked parameter is not handled, the processing will end-up at the “No” transition of the last choice-point. Also, if there is a larger number of possible values that need to be considered, the state diagram becomes cluttered.

Another way of doing this is to create an internal interface that defines a message for each possible value of the `status` parameter, which (in this example) is specified as an enum.

```cpp
enum StatusValue
{
    ACCEPTED, PENDING, REJECTED
};

FCM_SET_INTERFACE( Responses,
    FCM_DEFINE_MESSAGE( AcceptedInd );
    FCM_DEFINE_MESSAGE( PendingInd  );
    FCM_DEFINE_MESSAGE( RejectedInd );
    FCM_DEFINE_MESSAGE( ErrorInd    );
);
```

This interface can then be connected on both ends for the component.

```cpp  
connectInterface<Responses>(Requester, Requester);
```

This can be depicted in the component diagram as shown below.

![](https://www.plantuml.com/plantuml/img/JOun3i9030Hxly8b53o00YbL95fuWNKiAEBE5yvEGH1_nw8ufLkDPbSxc4lpfMIovwIpD2cSQfchGltgxmnkI2xwoDWe7s8-gwFT9M6ieKuh-CH7-XBjzdo1nOJ1YFfQmDS2Yn9HZtpOk3tzQPS7w2qU_W00)

Using the ‘internal’ interface, the (part of the) state diagram can be changed as shown below.

![](https://www.plantuml.com/plantuml/img/TPAzJWCn48JxVOfLIgGy05T4EKKWfKD72HIMlGg6tDhnRXG1ohl7zsDGk4lZdz5yzepbjQX9UcptyEuT8RLHlz2HsEIZ-i00ykavccHQh5CA9_m1nCV0-ko_2U_Xd1rgb73hzZGkwvFnwddNaCJ0GcW45qLyvI8lHljJ2fP4_WD_yXIfhQMevCe9n2xxPdSFENK2UBbxm-LgI0dTsEqcGIlS16Qo-UWjEu298KCpSIjiw70aqKOYm8nXsLqsPkxTWoxLRv4RqDZbYYdzRY0DVVG5hfWYpgTR3Q-KYNxkgyCwNvC_nWK0)

In the “RequestRsp” transition the value of the parameter is checked and the corresponding message is sent.

```cpp
switch message->status 
{
    case ACCEPTED:
        prepareMessage<Responses::AcceptedInd>(acceptedInd);
        sendMessage(acceptedInd);
        break;
    case PENDING:
        prepareMessage<Responses::PendingInd>(pendingInd);
        sendMessage(pendingInd);
        break;
    case REJECTED:
        prepareMessage<Responses::RejectedInd>(rejectedInd);
        sendMessage(rejectedInd);
        break;
    default:
        prepareMessage<Responses::ErrorInd>(errorInd);
        sendMessage(errorInd);
        break;
}
```

# Runtime errors

The following lists all the possible runtime errors that can occur.

<table>
  <tr>
   <td>Message
   </td>
   <td>Description
   </td>
  </tr>
  <tr>
   <td><code><em>State transition table is empty for component "&lt;component_name>"!</em></code>
   </td>
   <td>During the initialization of the functional component it was determined that the state-transition table is empty. This means that the overridden <code>setTransitions()</code> method is not properly implemented. 
   </td>
  </tr>
  <tr>
   <td><code><em>No states defined for component "&lt;component_name>"!</em></code>
   </td>
   <td>During the initialization of the functional component it was determined that no states are defined. This means that the overridden <code>setStates()</code> method is not properly implemented.
   </td>
  </tr>
  <tr>
   <td><code><em>State "&lt;state>" for component "&lt;component_name>" does not exist!</em></code>
   </td>
   <td>In the <code>setTransitions()</code> method a transition was added with a non-existing state.
   </td>
  </tr>
  <tr>
   <td><code><em>Next state "&lt;state>" for component "&lt;component_name" does not exist!</em></code>
   </td>
   <td>In the <code>setTransitions()</code> method a transition was added with a non-existing next-state.
   </td>
  </tr>
  <tr>
   <td><code><em>Transition "&lt;interface>:&lt;message_type>" on state "&lt;state>" already exists for component "&lt;component_name>!"</em></code>
   </td>
   <td>In the <code>setTransitions()</code> method a transition was added which was already part of the state-transition table.
   </td>
  </tr>
  <tr>
   <td><code><em>Choice-point "&lt;choice_point_name>" already exists for component "&lt;component_name>"!</em></code>
   </td>
   <td>In the <code>setChoicePoints()</code> method a choice-point was added which already existed.
   </td>
  </tr>
  <tr>
   <td><code><em>Component "&lt;sender_component_name>" sent the message "&lt;message_type>" to unconnected interface "&lt;interface>!"</em></code>
   </td>
   <td>The component tries to send a message to an unconnected interface. In most cases this happens when sending to a handler.
   </td>
  </tr>
  <tr>
   <td><code><em>Interface "&lt;interface>" of component "&lt;component_name" is already connected to component "&lt;remote_component_name>"!</em></code>
   </td>
   <td>It is tried to connect an interface that is already connected
   </td>
  </tr>
  <tr>
   <td><code><em>Component "&lt;component_name>" tries to send message "&lt;message_type>" to interface "&lt;interface>" on index &lt;index> but there are only &lt;number_of_connected_components> components connected!</em></code>
   </td>
   <td>The component tries to send a message to a multiplied interface on an index that is too large.
   </td>
  </tr>
  <tr>
   <td><code><em>Last received message cast to invalid message type "&lt;interface>:&lt;message_type>"!</em></code>
   </td>
   <td>Calling the <code>castLastReceivedMessage()</code> method.
   </td>
  </tr>
  <tr>
   <td><code><em>Messages on interface "&lt;interface>" in state "&lt;stateName>" of component "component_name" are not handled!</em></code>
   </td>
   <td>In the state-transition table, there are no transitions for the specified interface in the specified state.
   </td>
  </tr>
  <tr>
   <td><code><em>Message "&lt;message_type>" on interface "&lt;interface>" in state "&lt;state>" of component "&lt;component_name>" is not handled!</em></code>
   </td>
   <td>In the state-transition table, there is no transition for the specified message in the specified state.
   </td>
  </tr>
</table>

##
