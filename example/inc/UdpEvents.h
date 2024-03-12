//
// Created by Fred Dijkstra on 05/03/2024.
//

#ifndef FCM_UDPEVENTS_H
#define FCM_UDPEVENTS_H

#include "FcmMessage.h"

FCM_SET_INTERFACE(UdpEvents,

    FCM_DEFINE_MESSAGE( UdpMessageInd,
                        std::string message{}; );

    FCM_DEFINE_MESSAGE( EnabledInd );

    FCM_DEFINE_MESSAGE( DisabledInd );
);

#endif //FCM_UDPEVENTS_H
