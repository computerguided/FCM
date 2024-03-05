//
// Created by Fred Dijkstra on 05/03/2024.
//

#include "../inc/UdpHandler.h"


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
void UdpHandler::stopListening()
{
    listening = false;
    close(socketHandle);

    if (listenThread.joinable())
    {
        listenThread.join();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::send(const std::string& message)
{
    // Send the message
    if (sendto(socketHandle, message.c_str(), message.size(), 0, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        throw std::runtime_error("Failed to send message");
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::receive(std::string& message) const
{
    char buffer[1024];
    long n = recvfrom(socketHandle, buffer, 1024, 0, nullptr, nullptr); // Blocking call
    if (n < 0)
    {
        throw std::runtime_error("Failed to receive message");
    }
    buffer[n] = '\0';
    message = std::string(buffer);
}

// ---------------------------------------------------------------------------------------------------------------------
void UdpHandler::startListening()
{
    initSocket();
    listening = true;
    listenThread = std::thread([this]()
    {
        while (listening)
        {
            std::string message;
            receive(message); // Blocking call
            // TODO: Add message to message queue
        }
    });
}
