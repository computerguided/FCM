//
// Created by Fred Dijkstra on 25/02/2024.
//

#ifndef FCM_FCMDEVICE_H
#define FCM_FCMDEVICE_H

#include <map>

#include <FcmMessage.h>
#include <FcmStateMachineEngine.h>
#include <FcmTimerHandler.h>


// ---------------------------------------------------------------------------------------------------------------------
// FCM Device
// ---------------------------------------------------------------------------------------------------------------------
class FcmDevice
{
public:

    explicit FcmDevice(int timeStepMsParam);
    virtual void initialize() = 0;
    [[noreturn]] void run();

    std::shared_ptr<FcmMessageQueue> createNewMessageQueue();

private:

    std::shared_ptr<FcmMessageQueue> mainMessageQueue;
    std::vector<std::shared_ptr<FcmMessageQueue>> messageQueues;
    FcmStateMachineEngine stateMachineEngine;
    std::shared_ptr<FcmTimerHandler> timerHandler;
    const int timeStepMs;

    void processMessages();
    void copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue) const;
    void insertMessage(const std::shared_ptr<FcmMessage>& message) const;
};


#endif //FCM_FCMDEVICE_H
