//
// Created by Fred Dijkstra on 15/02/2024.
//

#ifndef FCM_PROTOTYPING_TRANSCEIVING_H
#define FCM_PROTOTYPING_TRANSCEIVING_H

#include <cstdint>
#include "FcmMessage.h"

#ifdef FCM_INTERFACE_NAME
#undef FCM_INTERFACE_NAME
#endif
#define FCM_INTERFACE_NAME "Transceiving"

namespace Transceiving
{
    FCM_DEFINE_MESSAGE( AdvertisementInd,
                        uint32_t clientId;
                        uint32_t serverId; );

    FCM_DEFINE_MESSAGE( ConnectedInd,
                        uint32_t connectionId; );

    FCM_DEFINE_MESSAGE( ConnectReq,
                        uint32_t serverId;
                        uint32_t connectionId; );

    FCM_DEFINE_MESSAGE( ConnectAck,
                        uint32_t connectionId; );

    FCM_DEFINE_MESSAGE( ConnectRej,
                        uint32_t connectionId; );
};

#endif //FCM_PROTOTYPING_TRANSCEIVING_H
