//
// Created by Fred Dijkstra on 22/01/2024.
//

#ifndef FCM_PROTOTYPING_FCMTIMERINTERFACE_H
#define FCM_PROTOTYPING_FCMTIMERINTERFACE_H

#include "FcmMessage.h"

#ifdef FCM_INTERFACE_NAME
#undef FCM_INTERFACE_NAME
#endif

#define FCM_INTERFACE_NAME "Timer"

namespace Timer
{
    const char* const currentNamespace = "Timer";
    FCM_DEFINE_MESSAGE( Timeout, int timerId;);
};

#endif //FCM_PROTOTYPING_FCMTIMERINTERFACE_H
