// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmAsyncInterfaceHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmAsyncInterfaceHandler::FcmAsyncInterfaceHandler(const std::string& nameParam,
                                                   const std::shared_ptr<FcmMessageQueue>& messageQueueParam)
    : name(nameParam), messageQueue(messageQueueParam)
{
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmAsyncInterfaceHandler::sendMessage(const std::shared_ptr<FcmMessage>& message)
{
    try
    {
        message->receiver = interfaces.at(message->interfaceName);
    }
    catch (const std::out_of_range& e)
    {
        message->receiver = nullptr;
    }

    messageQueue->push(message);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmAsyncInterfaceHandler::connectInterface(const std::string& interfaceName, FcmComponent* receiver)
{
    if (interfaces.find(interfaceName) != interfaces.end())
    {
        throw std::runtime_error("Interface " + interfaceName +
            " already connected " + " for handler " + name);
    }
    interfaces[interfaceName] = receiver;
}

