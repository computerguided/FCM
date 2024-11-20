#ifndef FCM_TIMER_HANDLER_H
#define FCM_TIMER_HANDLER_H

#include <map>
#include <thread>

#include <FcmMessage.h>
#include <FcmMessageQueue.h>

// ---------------------------------------------------------------------------------------------------------------------
using FcmTime = long long;

// ---------------------------------------------------------------------------------------------------------------------
struct FcmTimerInfo
{
    void* component;
    bool cancelled;
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

    [[nodiscard]] int setTimeout(FcmTime timeout, void* component);
    void cancelTimeout(int timerId);

private:  
    std::unordered_map<int, FcmTimerInfo> timeouts;
    std::mutex mutex;
    FcmMessageQueue& messageQueue;
    int nextTimerId{};

    void sendTimeoutMessage(int timerId, void* component);
    bool removeTimeoutMessage(int timerId);
};

#endif //FCM_TIMER_HANDLER_H

