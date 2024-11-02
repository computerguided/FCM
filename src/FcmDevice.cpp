#include <thread>
#include <chrono>
#include <iostream>

#include "FcmDevice.h"
#include "FcmFunctionalComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmDevice::FcmDevice(int timeStepMsParam) :
    messageQueue(FcmMessageQueue::getInstance()), 
    timerHandler(FcmTimerHandler::getInstance()), 
    timeStepMs(timeStepMsParam) 
{
}

// ---------------------------------------------------------------------------------------------------------------------
[[noreturn]] void FcmDevice::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        processMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(timeStepMs));
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        timerHandler.setCurrentTime(duration.count());
    }
}

//  ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::initializeComponents()
{
    for (const auto& component : components)
    {
        component->initialize();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::processMessages()
{
    std::optional<std::shared_ptr<FcmMessage>> message;
    while ((message = messageQueue.pop()).has_value())
    {
        auto receiver = (FcmFunctionalComponent*)message.value()->receiver;
        auto sender = (FcmBaseComponent*)message.value()->sender;

        if (receiver == nullptr)
        {
            throw std::runtime_error("Component \"" + sender->name +
                                    "\" sent the message \"" + message.value()->_name +
                                    "\" to unconnected interface \"" + message.value()->_interfaceName + "\"!");
        }

        receiver->processMessage(message.value());
    }
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::connectInterface(const std::string& interfaceName,
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