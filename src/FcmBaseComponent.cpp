#include <utility>

#include "FcmBaseComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmBaseComponent::FcmBaseComponent(std::string nameParam,
                                   const std::map<std::string, std::any> &settingsParam):
    name(std::move(nameParam)), 
    messageQueue(FcmMessageQueue::getInstance()), 
    settings(settingsParam)
{
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::connectInterface(const std::string& interfaceName,
                                        FcmBaseComponent *remoteComponent)
{
    auto& componentList = interfaces[interfaceName];

    if (std::find(componentList.begin(), componentList.end(), remoteComponent) != componentList.end())
    {
        throw std::runtime_error("Interface " + interfaceName +
                                 " already connected to supplied remote component");
    }

    componentList.push_back(remoteComponent);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::sendMessage(const std::shared_ptr<FcmMessage>& message, size_t index)
{
    try
    {
        auto& componentList = interfaces.at(message->interfaceName);
        if (index >= componentList.size())
        {
            throw std::out_of_range("Index out of range for interface components.");
        }
        message->receiver = componentList[index];

    }
    catch (const std::out_of_range& e)
    {
        message->receiver = nullptr;
    }

    messageQueue.push(message);
}