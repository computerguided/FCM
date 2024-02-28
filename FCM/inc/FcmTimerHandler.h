//
// Created by Fred Dijkstra on 25/02/2024.
//

#ifndef FCM_FCMTIMERHANDLER_H
#define FCM_FCMTIMERHANDLER_H

#include <map>
#include <FcmMessage.h>

// ---------------------------------------------------------------------------------------------------------------------
struct FcmTimerInfo
{
    int timerId;
    void* component;
};

// ---------------------------------------------------------------------------------------------------------------------
class FcmTimerHandler
{
private:
    std::multimap<long long, FcmTimerInfo> timeouts;
    long long currentTime;
    const std::shared_ptr<FcmMessageQueue> messageQueue;
    int nextTimerId;

public:

    explicit FcmTimerHandler(const std::shared_ptr<FcmMessageQueue>& messageQueueParam);

    void setCurrentTime(long long currentTimeParam);
    [[nodiscard]] int setTimeout(long long timeout, void* component);
    void cancelTimeout(int timerId);

private:

    static void sendTimeoutMessage(int timerId, void* component);
    void removeTimeoutMessage(int timerId);
};


#endif //FCM_FCMTIMERHANDLER_H
