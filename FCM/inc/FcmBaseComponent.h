//
// Created by Fred Dijkstra on 12/03/2024.
//

#ifndef FCM_FCMBASECOMPONENT_H
#define FCM_FCMBASECOMPONENT_H

#include <string>
#include <map>
#include <any>

#include "FcmMessage.h"
#include "FcmMessageQueue.h"

class FcmBaseComponent
{
public:
    const std::string name;
    const std::map<std::string, std::any> settings;

    FcmBaseComponent(const std::string &nameParam,
                     const std::shared_ptr<FcmMessageQueue> &messageQueueParam,
                     const std::map<std::string, std::any> &settingsParam);

    void connectInterface(const std::string &interfaceName, FcmBaseComponent *remoteComponent);
    void sendMessage(const std::shared_ptr<FcmMessage>& message);

    virtual ~FcmBaseComponent() = default;

protected:
    std::map<std::string, FcmBaseComponent*> interfaces;
    const std::shared_ptr<FcmMessageQueue> messageQueue;
};



#endif //FCM_FCMBASECOMPONENT_H
