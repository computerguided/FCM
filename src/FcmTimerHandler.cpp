#include "FcmTimerHandler.h"
#include "FcmFunctionalComponent.h"
#include <FcmMessageQueue.h>

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::setCurrentTime(FcmTime currentTimeParam)
{
    currentTime = currentTimeParam;

    if (timeouts.empty()) { return; }

    auto it = timeouts.begin();
    while (it != timeouts.end() && it->first <= currentTime)
    {
        auto component = static_cast<FcmFunctionalComponent*>(it->second.component);
        sendTimeoutMessage(it->second.timerId, component);
        it = timeouts.erase(it);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
int FcmTimerHandler::setTimeout(FcmTime timeout, void* component)
{
    FcmTime time = currentTime + timeout;
    FcmTimerInfo timerInfo = {nextTimerId++, component};
    timeouts.insert(std::make_pair(time, timerInfo));
    return timerInfo.timerId;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::sendTimeoutMessage(int timerId, void* component)
{
    FCM_PREPARE_MESSAGE(timeoutMessage, Timer, Timeout);
    timeoutMessage->timerId = timerId;
    timeoutMessage->receiver = component;
    messageQueue.push(timeoutMessage);
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmTimerHandler::cancelTimeout(int timerId)
{
    for (auto it = timeouts.begin(); it != timeouts.end(); ++it)
    {
        if (it->second.timerId != timerId) {continue;}
        timeouts.erase(it);
        return true;
    }
    return removeTimeoutMessage(timerId);
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmTimerHandler::removeTimeoutMessage(int timerId)
{
    auto checkFunction = [timerId](const std::shared_ptr<FcmMessage>& msg) -> bool
    {
        auto timeoutMessage = dynamic_cast<const Timer::Timeout*>(msg.get());
        return timeoutMessage && timeoutMessage->timerId != timerId;
    };

    return messageQueue.removeMessage("Timer", "Timeout", checkFunction);
}