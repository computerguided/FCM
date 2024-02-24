//
// Created by Fred Dijkstra on 22/01/2024.
//

#ifndef FCM_PROTOTYPING_FCMTIMERINTERFACE_H
#define FCM_PROTOTYPING_FCMTIMERINTERFACE_H

#include "FcmMessage.h"

namespace Timer
{
    // ------------------------------------------------------------------------
    // Messages
    // ------------------------------------------------------------------------
    class Timeout : public FcmMessage
    {
    };
};

#endif //FCM_PROTOTYPING_FCMTIMERINTERFACE_H
