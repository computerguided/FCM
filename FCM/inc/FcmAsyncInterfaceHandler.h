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
    const std::map<std::string, std::any> settings;

    FcmAsyncInterfaceHandler(const std::string& nameParam,
                             const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                             const std::map<std::string, std::any>& settingsParam);

    void connectInterface(const std::string& interfaceName, FcmComponent* receiver);

    virtual void initialize() = 0;

protected:
    const std::shared_ptr<FcmMessageQueue> messageQueue;
    std::map<std::string, FcmComponent*> interfaces;

    void sendMessage(const std::shared_ptr<FcmMessage>& message);
};

#endif //FCM_FCMASYNCINTERFACEHANDLER_H
