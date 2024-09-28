#include <thread>
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

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::connectInterface(const std::string& interfaceName,
                                 FcmBaseComponent* firstComponent,
                                 FcmBaseComponent* secondComponent)
{
    firstComponent->connectInterface(interfaceName, secondComponent);
    secondComponent->connectInterface(interfaceName, firstComponent);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::processMessages()
{
    std::optional<std::shared_ptr<FcmMessage>> message;
    while ((message = messageQueue.pop()).has_value())
    {
        auto receiver = (FcmFunctionalComponent*)(message->get()->receiver);

        if (receiver == nullptr)
        {
            // TODO: Log error.
            continue;
        }

        receiver->processMessage(message.value());
    }

}