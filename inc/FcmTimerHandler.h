#ifndef FCM_TIMER_HANDLER_H
#define FCM_TIMER_HANDLER_H

#include <map>

#include <FcmMessage.h>
#include <FcmMessageQueue.h>

// ---------------------------------------------------------------------------------------------------------------------
using FcmTime = long long;

// ---------------------------------------------------------------------------------------------------------------------
struct FcmTimerInfo
{
    int timerId;
    void* component;
};

// ---------------------------------------------------------------------------------------------------------------------
FCM_SET_INTERFACE(Timer, 
    FCM_DEFINE_MESSAGE( Timeout, int timerId{}; );
);

// ---------------------------------------------------------------------------------------------------------------------
class FcmTimerHandler
{
public:

    FcmTimerHandler() : messageQueue(FcmMessageQueue::getInstance()) {}
    FcmTimerHandler(const FcmTimerHandler&) = delete;
    FcmTimerHandler& operator=(const FcmTimerHandler&) = delete;

    static FcmTimerHandler& getInstance()
    {
        static FcmTimerHandler instance;
        return instance;
    }

    void setCurrentTime(FcmTime currentTimeParam);
    [[nodiscard]] int setTimeout(FcmTime timeout, void* component);
    bool cancelTimeout(int timerId);

private:  
    std::multimap<FcmTime, FcmTimerInfo> timeouts;
    FcmTime currentTime{};
    FcmMessageQueue& messageQueue;
    int nextTimerId{};

    void sendTimeoutMessage(int timerId, void* component);
    bool removeTimeoutMessage(int timerId);
};

#endif //FCM_TIMER_HANDLER_H

