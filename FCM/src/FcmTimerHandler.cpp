// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 25/02/2024.
// Documentation: https://github.com/computerguided/FCM/blob/main/FCM/doc/TimerHandler.md
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmTimerHandler.h"
#include "FcmComponent.h"
#include "FcmTimerInterface.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmTimerHandler::FcmTimerHandler(const std::shared_ptr<FcmMessageQueue>& messageQueueParam)
    : messageQueue(messageQueueParam)
{
}

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
void FcmTimerHandler::cancelTimeout(int timerId)
{
    // Find the timeout with the given timer id and remove it.
    for (auto it = timeouts.begin(); it != timeouts.end(); ++it)
    {
        if (it->second.timerId != timerId) {continue;}

        timeouts.erase(it);
        return; // Found it, erase it, so we can return.
    }

    removeTimeoutMessage(timerId);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::sendTimeoutMessage(int timerId, void* component)
{
    FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
    timeoutMessage->timerId = timerId;
    timeoutMessage->receiver = component;

    timeoutMessage->timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

    messageQueue->push(timeoutMessage);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::removeTimeoutMessage(int timerId)
{
    for (auto it = messageQueue->begin(); it != messageQueue->end(); ++it)
    {
        if ((*it)->interfaceName != "Timer") {continue;}

        auto timeoutMessage = dynamic_cast<Timer::Timeout*>(it->get());
        if (timeoutMessage->timerId != timerId) {continue;}

        messageQueue->erase(it);
        return;
    }
}