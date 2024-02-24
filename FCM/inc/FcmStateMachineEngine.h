//
// Created by Fred Dijkstra on 17/02/2024.
//

#ifndef FCM_PROTOTYPING_FCMSTATEMACHINEENGINE_H
#define FCM_PROTOTYPING_FCMSTATEMACHINEENGINE_H

// ---------------------------------------------------------------------------------------------------------------------
// Include files
// ---------------------------------------------------------------------------------------------------------------------
#include "FcmMessage.h"
#include "FcmComponent.h"


// ---------------------------------------------------------------------------------------------------------------------
// FcmStateMachineEngine
// ---------------------------------------------------------------------------------------------------------------------
class FcmStateMachineEngine
{
public:

    void handleMessage(const FcmMessage& message);
};


#endif //FCM_PROTOTYPING_FCMSTATEMACHINEENGINE_H
