//
// Created by Fred Dijkstra on 14/02/2024.
//

// ----------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <string>
#include "FcmComponent.h"

#ifndef FCM_PROTOTYPING_CONNECTOR_H
#define FCM_PROTOTYPING_CONNECTOR_H

class Connector : public FcmComponent
{
public:

    // ----------------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------------

    Connector(const std::string& name,
              const std::shared_ptr<FcmMessageQueue>& messageQueue,
              const std::shared_ptr<FcmTimerHandler>& timerHandlerParam,
              const std::map<std::string, std::any>& settingsParam);

    // ----------------------------------------------------------------------------
    // Settings
    // ----------------------------------------------------------------------------
    uint32_t clientId{};
    uint32_t connectionTimeout{};
    uint32_t advertisementInterval{};
    std::vector<uint32_t> serverWhitelist{};

    // ----------------------------------------------------------------------------
    // State variables
    // ----------------------------------------------------------------------------
    uint32_t connectionId{};
    uint32_t serverId{};
    int timerId{};

    // ----------------------------------------------------------------------------
    // Local functions
    // ----------------------------------------------------------------------------

    void advertise();
    void connectionOk();

    void setTransitions() override;
    void setChoicePoints() override;
};


#endif //FCM_PROTOTYPING_CONNECTOR_H
