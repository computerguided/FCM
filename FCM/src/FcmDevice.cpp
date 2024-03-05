//
// Created by Fred Dijkstra on 25/02/2024.
//

#include <thread>
#include "FcmDevice.h"
#include "FcmComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------------------------------------------
FcmDevice::FcmDevice(int timeStepMsParam) : timeStepMs(timeStepMsParam)
{
    mainMessageQueue = std::make_shared<FcmMessageQueue>();
    timerHandler = std::make_shared<FcmTimerHandler>(mainMessageQueue);
}

// ---------------------------------------------------------------------------------------------------------------------
// Run the device
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
        timerHandler->setCurrentTime(duration.count());
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Add new asynchronous message queue
// ---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<FcmMessageQueue> FcmDevice::createMessageQueue()
{
    auto messageQueue = std::make_shared<FcmMessageQueue>();
    messageQueues.push_back(messageQueue);
    return messageQueue;
}

// ---------------------------------------------------------------------------------------------------------------------
// Process messages
// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::processMessages()
{
    for (auto& messageQueue : messageQueues)
    {
        copyMessages(messageQueue);
    }

    while (!mainMessageQueue->empty())
    {
        auto message = mainMessageQueue->pop();

        // Get the receiver of the message.
        auto receiver = (FcmComponent*)message->receiver;

        if (receiver == nullptr)
        {
            // TODO: Log error.
            continue;
        }

        receiver->processMessage(message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Copy the messages from a message queue to the main message queue
// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue) const
{
    while (!messageQueue->empty())
    {
        auto message = messageQueue->pop();
        mainMessageQueue->push(message);
    }
}