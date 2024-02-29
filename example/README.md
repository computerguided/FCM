# Functional Components Method Example
---
This section provides an example of how to use the FCM methods.

---

## Description
The Connector takes care of managing the connection to a Server. It uses a "Transceiver" interface to exchange messages with Servers.

## Behavior
When the Client is not connected, it advertises its presence by sending "AdvertismentInd" messages containing the Client ID at regular intervals.

When the "AdvertisementInd" is detected by a Server, the Server sends a "ConnectReq" message containing its own Server ID and a unique connection ID.

Upon receiving the "ConnectReq", the Client stops advertising and checks the received Client ID and Server ID.
When the Server ID is not part of the whitelist from the settings, the connection is refused by the Client by responding with a "ConnectRej" containing the received connection ID, after which it starts advertising again. If the Server ID is correct, the Client responds with a "ConnectAck" message to the Server containing the connection ID. The Client then starts a timer to await the first "ConnectedInd" from the Server.

Upon receiving the "ConnectAck" message, the Server will start sending regular "ConnectedInd" messages containing the connection ID.

Upon receiving a "ConnectedInd" message from the Server, the Client will restart its connection timer and respond with a "ConnectedInd" message. When the timer expires, the Client will start advertising again.

Settings
Parameter
Type
Description
clientId
uint32_t
The ID of the Client itself.
serverWhitelist
[uint32_t]
The list of servers that the Client is allowed to connect to.
connectionTimeout
uint32_t
Maximum number of milliseconds allowed between two consecutive "ConnectedInd" messages.
advertisementInterval
uint32_t
Number of milliseconds between two "AdvertisementInd" messages.

State variables
Variable
Type
Description
serverId
uint32_t
The ID of the Server that the Client is connected to or about to connect to.
connectionId
uint32_t
The ID of the connection.
timerId
uint32_t
The ID of the timer in order for it to be stopped and restarted.


## States

| State       | Description                                                                                                         |
|-------------|---------------------------------------------------------------------------------------------------------------------|
| Advertising | The Client is not connected and is advertising its presence.                                                        |
| Connecting  | The connection request is accepted and the first "ConnectedInd" is awaited.                                         |
| Connected   | The connection has been established. The connection timer is running to monitor whether the connection is not lost. |

## Choice-points

| Choice-point    | Evaluation                                                              |
|-----------------|-------------------------------------------------------------------------|
| Correct Server? | Whether the serverId is in the serverWhitelist and clientId is correct. |

## State diagram

![state-diagram](https://www.plantuml.com/plantuml/img/TP712i8m44Jl-nMBHw4_q8DMWWSlAgKNKP7IB1gmIKssWdzlXDQaBRs5dRSpioHpN5fkwoSyb2Iakk4FIdALLGqhem4ym8GR-IRBoYbzxoR2Q4qLJmOa-nrnl8b3dcz5WQN3XJ3MUlsY7Lbljbu0m7bvnIHP3UoZzrZ355dLP5g62HQbfNsNd5u0aV33_hKzTY8taeTXU-FNmqBIDi61PCWXkTCIWZ2jNNIrXyoirI1rt3ewBiRc8ZFIqd_m5m00)


## State transition table


| State             | Interface    | Message      | Next state      | Action                                                                            |
|-------------------|--------------|--------------|-----------------|-----------------------------------------------------------------------------------|
| Advertising       | Timer        | Timeout      | Advertising     | Call ``advertise()``.                                                             |
|                   | Transceiving | ConnectReq   | Correct Server? | Stop timer.<br>Set ``serverId``.<br>                                              |
| Set connectionId. |              |              |                 |                                                                                   |
| Correct Server?   | Logical      | Yes          | Connecting      | Send an "ConnectAck" message.<br>Start timer with ``settings.connectionTimeout``. |
|                   |              | No           | Advertising     | Send "ConnectRej" message.<br>Call ``advertise()``.                               |
| Connecting        | Timer        | Timeout      | Advertising     | Call ``advertise()``.                                                             |
|                   | Transceiving | ConnectReq   | Connecting      | -                                                                                 |
|                   |              | ConnectedInd | Connected       | Call ``connectionOk()``.                                                          |
| Connected         | Timer        | Timeout      | Advertising     | Call ``advertise()``.                                                             |
|                   | Transceiving | ConnectedInd | Connected       | Call ``connectionOk()``.                                                          |

## Local functions

| Function           | Actions                                                                                       |
|--------------------|-----------------------------------------------------------------------------------------------|
| ``advertise()``    | Send an "AdvertisementInd" message.<br>Restart timer with ``settings.advertisementInterval``. |
| ``connectionOk()`` | Stop timer.<br>Send "ConnectedInd".<br>Start timer with ``settings.connectionTimeout``.       |


