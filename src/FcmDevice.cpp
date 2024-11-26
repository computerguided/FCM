#include "FcmDevice.h"
#include "FcmFunctionalComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmDevice::FcmDevice() :
    messageQueue(FcmMessageQueue::getInstance())
{
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::run()
{
    while (true)
    {
        auto message = messageQueue.awaitMessage();
        processMessages(message);
    }
}

//  ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::initializeComponents()
{
    for (const auto& component : components)
    {
        component->_initialize();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::processMessages(std::shared_ptr<FcmMessage>& message)
{
    auto receiver = (FcmFunctionalComponent*)message->receiver;
    auto sender = (FcmBaseComponent*)message->sender;

    if (receiver == nullptr)
    {
        auto errorMessage = "Sent the message \"" + message->_name +
                            "\" to unconnected interface \"" + message->_interfaceName + "\"!";
        sender->logError(errorMessage);
        return;
    }
    receiver->processMessage(message);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::_connectInterface(const std::string& interfaceName,
                                  FcmBaseComponent* firstComponent,
                                  FcmBaseComponent* secondComponent)
{
    if (dynamic_cast<FcmFunctionalComponent*>(secondComponent) != nullptr)
    {
        firstComponent->connectInterface(interfaceName, secondComponent);
    }

    if (dynamic_cast<FcmFunctionalComponent*>(firstComponent) != nullptr)
    {
        secondComponent->connectInterface(interfaceName, firstComponent);
    }
}