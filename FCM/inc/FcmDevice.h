//
// Created by Fred Dijkstra on 25/02/2024.
//

#ifndef FCM_FCMDEVICE_H
#define FCM_FCMDEVICE_H

#include <map>

#include <FcmMessage.h>
#include <FcmTimerHandler.h>
#include <FcmMessageQueue.h>
#include <FcmAsyncInterfaceHandler.h>


// ---------------------------------------------------------------------------------------------------------------------
// FCM Device
// ---------------------------------------------------------------------------------------------------------------------
class FcmDevice
{
public:

    explicit FcmDevice(int timeStepMsParam);
    virtual void initialize() = 0;
    [[noreturn]] void run();

    std::shared_ptr<FcmMessageQueue> createMessageQueue();

private:

    std::shared_ptr<FcmMessageQueue> mainMessageQueue;
    std::vector<std::shared_ptr<FcmMessageQueue>> messageQueues;
    std::shared_ptr<FcmTimerHandler> timerHandler;
    const int timeStepMs;

    void processMessages();
    void copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue) const;
};


#endif //FCM_FCMDEVICE_H
