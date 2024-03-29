// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 12/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmBaseComponent.h"

#include <utility>

// ---------------------------------------------------------------------------------------------------------------------
FcmBaseComponent::FcmBaseComponent(std::string nameParam,
                                   const std::shared_ptr<FcmMessageQueue> &messageQueueParam,
                                   const std::map<std::string, std::any> &settingsParam):
                                   name(std::move(nameParam)), messageQueue(messageQueueParam), settings(settingsParam)
{
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::connectInterface(const std::string& interfaceName,
                                        FcmBaseComponent *remoteComponent)
{
    if (interfaces.find(interfaceName) != interfaces.end())
    {
        throw std::runtime_error("Interface " + interfaceName +
                                 " already connected " + " for " + name);
    }

    interfaces[interfaceName] = remoteComponent;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::sendMessage(const std::shared_ptr<FcmMessage>& message)
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
