//
// Created by Fred Dijkstra on 05/03/2024.
//

#include "../inc/UdpHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
UdpHandler::UdpHandler(const std::string& nameParam,
                       const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                       const std::map<std::string, std::any>& settingsParam)
    : FcmAsyncInterfaceHandler(nameParam, messageQueueParam, settingsParam)
{
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::initialize()
{
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::initSocket()
{
    // Initialize the socket
    socketHandle = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketHandle < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(UDP_PORT);

    if (bind(socketHandle, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        throw std::runtime_error("Failed to bind socket");
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::enable()
{
    listening = true;
    initSocket();

    listenThread = std::thread([this]()
    {
       while (listening)
       {
           receive();
       }
    });

    FCM_PREPARE_MESSAGE(udpEnabledInd, UdpEvents, EnabledInd);
    FCM_SEND_MESSAGE(udpEnabledInd);
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::disable()
{
    listening = false;
    close(socketHandle);

    if (listenThread.joinable())
    {
        listenThread.join();
    }

    FCM_PREPARE_MESSAGE(udpDisabledInd, UdpEvents, DisabledInd);
    FCM_SEND_MESSAGE(udpDisabledInd);
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::send(const std::string& message)
{
    auto result = sendto(socketHandle, message.c_str(), message.size(), 0,
                         (const struct sockaddr *)&serverAddress,
                                 sizeof(serverAddress));

    if (result < 0)
    {
        throw std::runtime_error("Failed to send message");
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::receive()
{
    char buffer[1024];
    long n = recvfrom(socketHandle, buffer, 1024, 0, nullptr, nullptr); // Blocking call
    if (n < 0)
    {
        if (errno == EBADF)
        {
            // Socket was closed, handle this as a special case
            return;
        }
        else
        {
            throw std::runtime_error("Failed to receive message");
        }
    }
    buffer[n] = '\0';

    FCM_PREPARE_MESSAGE(udpMessageInd, UdpEvents, UdpMessageInd);
    udpMessageInd->message = std::string(buffer);
    FCM_SEND_MESSAGE(udpMessageInd);
}


