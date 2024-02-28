//
// Created by Fred Dijkstra on 14/02/2024.
//

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include <string>
#include "FcmComponent.h"

#ifndef FCM_PROTOTYPING_CONNECTOR_H
#define FCM_PROTOTYPING_CONNECTOR_H

class Connector : public FcmComponent
{
public:
    struct Settings
    {
        // The ID of the Client itself.
        uint32_t clientId;

        // The list of servers that the Client is allowed to connect to.
        std::vector<uint32_t> serverWhitelist;

        // The maximum number of milliseconds allowed between two consecutive "ConnectedInd" messages.
        uint32_t connectionTimeout; // Maximum number of milliseconds allowed between two consecutive "ConnectedInd" messages.

        // Number of milliseconds between two "AdvertisementInd" messages.
        uint32_t advertisementInterval;
    };

    const std::shared_ptr<Settings> settings;

    // ----------------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------------
    Connector(std::string& name,
              const std::shared_ptr<FcmMessageQueue>& messageQueue,
              const std::shared_ptr<Settings>& settingsParam,
              const std::shared_ptr<FcmTimerHandler>& timerHandlerParam) :
        FcmComponent(name, messageQueue, timerHandlerParam),
        settings(settingsParam),
        clientId(settings->clientId),
        serverId(0),
        connectionId(0){};

    // ----------------------------------------------------------------------------
    // State variables
    // ----------------------------------------------------------------------------
    uint32_t clientId;
    uint32_t serverId;
    uint32_t connectionId;
    int timerId;

    // ----------------------------------------------------------------------------
    // Local functions
    // ----------------------------------------------------------------------------

    void advertise();
    void connectionOk();

    void setTransitions() override;
    void setChoicePoints() override;
};


#endif //FCM_PROTOTYPING_CONNECTOR_H
