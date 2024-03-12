// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef FCM_FCMASYNCINTERFACEHANDLER_H
#define FCM_FCMASYNCINTERFACEHANDLER_H

#include <string>

#include "FcmBaseComponent.h"
#include "FcmMessage.h"
#include "FcmMessageQueue.h"
#include "FcmComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmAsyncInterfaceHandler: public FcmBaseComponent
{
public:
    FcmAsyncInterfaceHandler(const std::string& nameParam,
                             const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                             const std::map<std::string, std::any>& settingsParam);

    virtual void initialize() = 0;
};

#endif //FCM_FCMASYNCINTERFACEHANDLER_H
