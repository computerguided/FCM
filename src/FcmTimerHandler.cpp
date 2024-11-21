#include "FcmTimerHandler.h"
#include "FcmFunctionalComponent.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
int FcmTimerHandler::setTimeout(FcmTime timeout, void* component)
{
    int timerId = nextTimerId++;

    std::lock_guard<std::mutex> lock(mutex);
    timeouts.emplace(std::make_pair(timerId, FcmTimerInfo{component, false}));

    std::thread([this, timerId, timeout]() 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        std::lock_guard<std::mutex> lock(mutex);
        if (!timeouts[timerId].cancelled)
        {
            sendTimeoutMessage(timerId, timeouts[timerId].component);
        }
        timeouts.erase(timerId);
    }).detach();

    return timerId;
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
void FcmTimerHandler::cancelTimeout(int timerId)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (timeouts.find(timerId) != timeouts.end())
    {
        timeouts[timerId].cancelled = true;
        return;
    }

    removeTimeoutMessage(timerId);
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