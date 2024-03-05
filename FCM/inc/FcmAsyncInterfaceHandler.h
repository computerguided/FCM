// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef FCM_FCMASYNCINTERFACEHANDLER_H
#define FCM_FCMASYNCINTERFACEHANDLER_H

#include <string>

#include "FcmMessage.h"
#include "FcmMessageQueue.h"
#include "FcmComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmAsyncInterfaceHandler
{
public:
    const std::string name;

    FcmAsyncInterfaceHandler(std::string& nameParam,
                            const std::shared_ptr<FcmMessageQueue>& messageQueueParam);

    void connectInterface(const std::string& interfaceName, FcmComponent* remoteComponent);

    virtual void initialize() = 0;

protected:
    const std::shared_ptr<FcmMessageQueue> messageQueue;
    std::map<std::string, FcmComponent*> interfaces;

    void initSocket();
    void sendMessage(const std::shared_ptr<FcmMessage>& message);
};

#endif //FCM_FCMASYNCINTERFACEHANDLER_H
