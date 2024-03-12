// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#include "ClientDevice.h"
#include "UdpHandler.h"
#include "Connector.h"

// ---------------------------------------------------------------------------------------------------------------------
void ClientDevice::initialize()
{
    // Create asynchronous message queues
    auto udpEventsMessageQueue = createMessageQueue();

    // Create asynchronous interface handlers
    auto settings = std::map<std::string, std::any>();
//    auto udpHandler = std::make_shared<UdpHandler>("udpHandler", udpEventsMessageQueue, settings);

    // Create components
    auto settings1 = std::map<std::string, std::any>();

    auto component1 = createComponent<Connector>("Connector", mainMessageQueue, settings1);

    // Connect components
//    connectInterface("udp", component1.get(), udpHandler.get());






}