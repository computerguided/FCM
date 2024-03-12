## Server component

## Description

The Server component is responsible for listening for advertisement indications from clients and establishing connections with them. It is also responsible for monitoring the connections and detecting when they are lost.

## Behavior
A Server can maintain a connection with a specified limited number of Clients.

When the Server is not connected, it listens for "AdvertisementInd" messages from clients. When the "AdvertisementInd" is detected by the Server, the Server sends a "ConnectReq" message containing its own Server ID and a unique connection ID.

The Client can either accept or reject the connection request. If the Client rejects the connection request, the Server will start listening for "AdvertisementInd" messages again. If the Client accepts the connection request, the Server will start sending regular "ConnectedInd" messages containing the connection ID.

The Server will maintain a list of active connections.

While connected to one or more Clients, the Server can accept new Clients until the maximum number of connections is reached. When the maximum number of connections is reached, the Server will no longer reject to "AdvertisementInd" messages from Clients.

When the Server is connected to one or more Clients, it will monitor the connections by sending regular "ConnectedInd" messages. If the Server does not receive a "ConnectedInd" message from a Client within a specified time, the Server will close the connection and remove the Client from the list of active connections.

## Settings

| Parameter             | Type         | Description                                                                             |
|-----------------------|--------------|-----------------------------------------------------------------------------------------|
| ``serverId``          | ``uint32_t`` | The ID of the Server itself.                                                            |
| ``connectionTimeout`` | ``uint32_t`` | Maximum number of milliseconds allowed between two consecutive "ConnectedInd" messages. |
| ``maxConnections``    | ``uint32_t`` | The maximum number of connections that the Server can handle.                           |

## State variables

| Variable              | Type                 | Description                                                      |
|-----------------------|----------------------|------------------------------------------------------------------|
| ``activeConnections`` | ``[ConnectionInfo]`` | The list of connections that the Server is currently handling.   |
| ``timerId``           | ``uint32_t``         | The ID of the timer in order for it to be stopped and restarted. |
| ``currentConnection`` | ``uint32_t``         | The ID of the current connection that is being evaluated.        |

The `ConnectionInfo` struct is defined as follows:

| Field            | Type         | Description                             |
|------------------|--------------|-----------------------------------------|
| ``connectionId`` | ``uint32_t`` | The ID of the connection.               |
| ``clientId``     | ``uint32_t`` | The ID of the client.                   |
 | ``connectionOk`` | ``bool``     | Whether the connection is still active. |

## States

| State      | Description                                                                                                         |
|------------|---------------------------------------------------------------------------------------------------------------------|
| Listening  | The Server is not connected and is listening for "AdvertisementInd" messages.                                       |
| Connecting | The connection request is accepted and the first "ConnectedInd" is awaited.                                         |
| Connected  | The connection has been established. The connection timer is running to monitor whether the connection is not lost. |

## Choice-points

| Choice-point              | Evaluation                                                   |
|---------------------------|--------------------------------------------------------------|
| Connection OK?            | Whether the connection is still active.                      |
| Evaluate next connection? | Whether the next connection in the list should be evaluated. |
| Max connections reached?  | Whether the maximum number of connections has been reached.  |
| Clients connected?        | Whether there are clients connected.                         |

## State diagram

![state_diagram](https://www.plantuml.com/plantuml/png/VPB1JiCm38RlUGfhHwI-G0zbqyG10Hj2N115g6ei5jOwK-DEu-s9ehKEQSJD_VFzlnqtQyVDm6FVgOFH2DYV-0SqkdOm9pQMbF808pmQnqY6lgxVMqk4BIy4r6eIdZydoDB-28s3bGWrxHzkLuBUdPjkzFKEBnpGIG3oIatjx8nTJyr58btmUIccNKq3Dkq1TUpRTWQ9tJnpS5x5sHqiwltc0uegklzSUJbQ0vImqMSSs3ZiaVYUj8fE2u5HGpdLc_RuXwfoqmd-WjzAv2HJwj1Ri2gpgloq4hpu9oMycXxjo6ePbipnXYu7fUlQMPKsyKZoR2Gk1QLZY4fzooj8L0AcEvVGt47nJtAUbXSJGjKQIOzzzmi0)


****

