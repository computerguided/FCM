#ifndef FCM_TIMER_HANDLER_H
#define FCM_TIMER_HANDLER_H

#include <map>
#include <thread>

#include <FcmMessage.h>
#include <FcmMessageQueue.h>

// ---------------------------------------------------------------------------------------------------------------------
using FcmTime = long long;
using FcmTimerID = int;

// ---------------------------------------------------------------------------------------------------------------------
struct FcmTimerInfo
{
    void* component;
    bool cancelled;
};

// ---------------------------------------------------------------------------------------------------------------------
FCM_SET_INTERFACE(Timer, 
    FCM_DEFINE_MESSAGE( Timeout, FcmTimerID timerId{}; );
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

    void setTimeout(FcmTimerID& timerId, FcmTime timeout, void* component);
    void cancelTimeout(FcmTimerID timerId);

private:  
    std::unordered_map<FcmTimerID, FcmTimerInfo> timeouts;
    std::mutex mutex;
    FcmMessageQueue& messageQueue;
    int nextTimerId{};

    void sendTimeoutMessage(FcmTimerID timerId, void* component);
    bool removeTimeoutMessage(FcmTimerID timerId);
};

#endif //FCM_TIMER_HANDLER_H

