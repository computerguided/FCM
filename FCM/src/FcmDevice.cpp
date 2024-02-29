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
// Create a new message queue
// ---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<FcmMessageQueue> FcmDevice::createNewMessageQueue()
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
        auto message = mainMessageQueue->front();

        // Get the receiver of the message.
        auto receiver = (FcmComponent*)message->receiver;

        if (receiver == nullptr)
        {
            // TODO: Log error.
            continue;
        }

        receiver->processMessage(*message);
        mainMessageQueue->pop_front();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Copy the messages from a message queue to the main message queue
// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue) const
{
    while (!messageQueue->empty())
    {
        insertMessage(messageQueue->front());
        messageQueue->pop_front();
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Insert a message in the main message queue at the proper position depending on the time of the message
// ---------------------------------------------------------------------------------------------------------------------
void FcmDevice::insertMessage(const std::shared_ptr<FcmMessage>& message) const
{
    auto it = std::find_if(mainMessageQueue->begin(), mainMessageQueue->end(),
        [&message](const std::shared_ptr<FcmMessage>& m)
        {
           return m->timestamp > message->timestamp;
        });

    mainMessageQueue->insert(it, message);
}