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

    if (timeouts.empty()) { return; }

    // Loop through the timeouts and handle the ones that have expired. The list is
    // sorted, so we need not loop through all of them.
    auto it = timeouts.begin();
    while (it != timeouts.end() && it->first <= currentTime)
    {
        auto component = static_cast<FcmComponent*>(it->second.component);
        sendTimeoutMessage(it->second.timerId, component);
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
// Cancel a timeout
// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::cancelTimeout(int timerId)
{
    // Find the timeout with the given timer id and remove it.
    for (auto it = timeouts.begin(); it != timeouts.end(); ++it)
    {
        if (it->second.timerId == timerId)
        {
            timeouts.erase(it);
            return; // Found it, erase it, so we can return.
        }
    }

    // If the timeout was not found, it is possible that the timeout has already
    // expired and been handled. In that case, the timeout message is in the
    // message queue. We need to remove it from the message queue.

    removeTimeoutMessage(timerId);
}

// ---------------------------------------------------------------------------------------------------------------------
// Send a timeout message
// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::sendTimeoutMessage(int timerId, void* component)
{
    // Create a timeout message and have the component send it to itself.
    FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
    timeoutMessage->timerId = timerId;
    auto* castedComponent = static_cast<FcmComponent*>(component);
    castedComponent->sendMessage(timeoutMessage);
}

// ---------------------------------------------------------------------------------------------------------------------
// Remove a timeout message from the message queue
// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::removeTimeoutMessage(int timerId)
{
    for (auto it = messageQueue->begin(); it != messageQueue->end(); ++it)
    {
        if ((*it)->interfaceName != "Timer") {continue;}

        // Cast to "Timeout" message to access the timer id.
        auto timeoutMessage = dynamic_cast<Timer::Timeout*>(it->get());
        if (timeoutMessage->timerId != timerId) { continue;}

        messageQueue->erase(it);
        return;
    }
}