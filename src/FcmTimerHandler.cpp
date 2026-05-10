#include "FcmTimerHandler.h"
#include "FcmFunctionalComponent.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::setTimeout(FcmTimerID& timerId, FcmTime timeout, void* component)
{
    timerId = nextTimerId++;

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
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::sendTimeoutMessage(FcmTimerID timerId, void* component)
{
    auto timeoutMessage = std::make_shared<Timer::Timeout>();
    timeoutMessage->timerId = timerId;
    timeoutMessage->receiver = component;
    messageQueue.push(timeoutMessage);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmTimerHandler::cancelTimeout(FcmTimerID timerId)
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
bool FcmTimerHandler::removeTimeoutMessage(FcmTimerID timerId)
{
    auto checkFunction = [timerId](const std::shared_ptr<FcmMessage>& msg) -> bool
    {
        auto timeoutMessage = static_cast<const Timer::Timeout*>(msg.get());
        return timeoutMessage && timeoutMessage->timerId != timerId;
    };

    return messageQueue.removeMessage("Timer", "Timeout", checkFunction);
}