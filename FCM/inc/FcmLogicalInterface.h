//
// Created by Fred Dijkstra on 22/01/2024.
//

#ifndef FCM_PROTOTYPING_FCMLOGICALINTERFACE_H
#define FCM_PROTOTYPING_FCMLOGICALINTERFACE_H


#include "FcmMessage.h"

#ifdef FCM_INTERFACE_NAME
#undef FCM_INTERFACE_NAME
#endif

#define FCM_INTERFACE_NAME "Logical"

namespace Logical
{
    const char* const currentNamespace = "Logical";
    FCM_DEFINE_MESSAGE( Yes );
    FCM_DEFINE_MESSAGE( No );
};


#endif //FCM_PROTOTYPING_FCMLOGICALINTERFACE_H
