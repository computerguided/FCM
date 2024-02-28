//
// Created by Fred Dijkstra on 25/02/2024.
//

#include "FcmTimerHandler.h"
#include "FcmComponent.h"
#include "FcmTimerInterface.h"

// ---------------------------------------------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------------------------------------------
FcmTimerHandler::FcmTimerHandler(const std::shared_ptr<FcmMessageQueue>& messageQueueParam)
    : messageQueue(messageQueueParam)
{
    currentTime = 0;
    nextTimerId = 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// Set the current time
// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::setCurrentTime(long long currentTimeParam)
{
    currentTime = currentTimeParam;

    if (timeouts.empty())
    {
        return;
    }

    // Loop through the timeouts and handle the ones that have expired. The list is
    // sorted, so we need not loop through all of them.
    auto it = timeouts.begin();
    while (it != timeouts.end() && it->first <= currentTime)
    {
        auto component = static_cast<FcmComponent*>(it->second.component);

        // Create a timeout message and have the component sent it to itself.
        FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
        component->sendMessage(timeoutMessage);
        it = timeouts.erase(it);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Set a timeout for a component
// ---------------------------------------------------------------------------------------------------------------------
int FcmTimerHandler::setTimeout(long long timeout, void* component)
{

    // Find the time of the timeout
    long long time = currentTime + timeout;

    // Create a timer info object
    FcmTimerInfo timerInfo = {nextTimerId++, component};

    // Add the timeout to the list of timeouts.
    timeouts.insert(std::make_pair(time, timerInfo));

    // Return the timer id
    return timerInfo.timerId;
}

// ---------------------------------------------------------------------------------------------------------------------
// Clear a timeout
// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::clearTimeout(int timerId)
{
    // Find the timeout with the given timer id and remove it.
    FcmComponent* component = nullptr;
    for (auto it = timeouts.begin(); it != timeouts.end(); ++it)
    {
        if (it->second.timerId == timerId)
        {
            component = static_cast<FcmComponent*>(it->second.component);
            timeouts.erase(it);
            break;
        }
    }

    // Search the message queue for the timeout message and remove it
    for (auto it = messageQueue->begin(); it != messageQueue->end(); ++it)
    {
        if ((*it)->interfaceName == "Timer" &&
            (*it)->name == "Timeout" &&
            (*it)->receiver == component)
        {
            // Cast to "Timeout" message and remove it.
            auto timeoutMessage = dynamic_cast<Timer::Timeout*>(it->get());

            // Remove if correct timer id
            if (timeoutMessage->timerId == timerId)
            {
                messageQueue->erase(it);
                break;
            }
        }
    }
}
