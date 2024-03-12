# UDP Handler
****
_The "UDP Handler" is responsible for handling the UDP communication._
****

## Description

The "UDP Handler" is responsible for handling the UDP communication.

It has a "UdpEvents" interface to send and receive messages.

## Interface functions

The "UDP Handler" has the following interface functions:

| Function    | Description                                |
|:-------------|:--------------------------------------------|
| `enable()`  | Enable the UDP communication.              |
| `disable()` | Disable the UDP communication.             |
| `send()`    | Send the supplied string as a UDP message. |

## Enable

To start the UDP communication, the `enable()` function must be called.

```cpp
void enable()
```

The first step is to set the ``listening`` variable to ``true``.

```cpp
listening = true;
```

The next step is initializing the socket by calling the `initializeSocket()` function.

```cpp
initializeSocket();
```

The last step is to start the listening by calling the `startListening()` function.

```cpp
startListening();
```





