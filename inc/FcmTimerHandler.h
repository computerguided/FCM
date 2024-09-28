#ifndef FCM_TIMER_HANDLER_H
#define FCM_TIMER_HANDLER_H

#include <map>

#include <FcmMessage.h>
#include <FcmMessageQueue.h>

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

    static FcmTimerHandler& getInstance()
    {
        static FcmTimerHandler instance;
        return instance;
    }

    void setCurrentTime(long long currentTimeParam);
    [[nodiscard]] int setTimeout(long long timeout, void* component);
    void cancelTimeout(int timerId);

private:  
    std::multimap<long long, FcmTimerInfo> timeouts;
    long long currentTime{};
    FcmMessageQueue& messageQueue;
    int nextTimerId{};

    FcmTimerHandler() : messageQueue(FcmMessageQueue::getInstance()) {}
    FcmTimerHandler(const FcmTimerHandler&) = delete;
    FcmTimerHandler& operator=(const FcmTimerHandler&) = delete;

    void sendTimeoutMessage(int timerId, void* component);
    void removeTimeoutMessage(int timerId);
};

#endif //FCM_TIMER_HANDLER_H

