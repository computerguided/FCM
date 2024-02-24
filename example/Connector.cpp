//
// Created by Fred Dijkstra on 14/02/2024.
//

#include "Connector.h"
#include "Transceiving.h"

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
Connector::Connector(std::string& name,
                     const std::shared_ptr<FcmMessageQueue>& messageQueue,
                     const std::shared_ptr<Settings>& settingsParam) :
    FcmComponent(name, messageQueue),
    settings(settingsParam),
    clientId(settings->clientId),
    serverId(0),
    connectionId(0)
{

    // ----------------------------------------------------------------------------
    // State Transition Table
    // ----------------------------------------------------------------------------
    FCM_ADD_TRANSITION( "Advertising", Timer, Timeout, "Advertising",
    {
        advertise();
    });

    FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Correct Server?",
    {
        // TODO: stop timer
        serverId = message.serverId;
        connectionId = message.connectionId;
    });

    FCM_ADD_TRANSITION( "Correct Server?", Logical, Yes, "Connecting",
    {
        FCM_PREPARE_MESSAGE(connectAck, Transceiving, ConnectAck);
        connectAck->connectionId = connectionId;
        FCM_SEND_MESSAGE(connectAck);

        // TODO: Start timer with settings.connectionTimeout.
    });

    FCM_ADD_TRANSITION( "Correct Server?", Logical, No, "Advertising",
    {
        FCM_PREPARE_MESSAGE(connectRej, Transceiving, ConnectRej);
        connectRej->connectionId = connectionId;
        FCM_SEND_MESSAGE(connectRej);
        advertise();
    });

    FCM_ADD_TRANSITION( "Connecting", Timer, Timeout, "Advertising",
    {
        advertise();
    });

    FCM_ADD_TRANSITION( "Connecting", Transceiving, ConnectReq, "Connecting",
    {
        // NOP
    });

    FCM_ADD_TRANSITION( "Connecting", Transceiving, ConnectedInd, "Connected",
    {
        connectionOk();
    });

    FCM_ADD_TRANSITION( "Connected", Timer, Timeout, "Advertising",
    {
        advertise();
    });

    FCM_ADD_TRANSITION( "Connected", Transceiving, ConnectedInd, "Connected",
    {
        connectionOk();
    });

    // ----------------------------------------------------------------------------
    // Choice Point Table
    // ----------------------------------------------------------------------------

    FCM_ADD_CHOICE_POINT( "Correct Server?",
    {
        // Determine whether the server is in the whitelist.
        auto it = std::find(settings->serverWhitelist.begin(),
                            settings->serverWhitelist.end(), serverId);

        return it != settings->serverWhitelist.end();
    });
}

// ----------------------------------------------------------------------------
// Advertise
// ----------------------------------------------------------------------------
void Connector::advertise()
{
    FCM_PREPARE_MESSAGE(advertisementInd, Transceiving, AdvertisementInd);
    advertisementInd->clientId = clientId;
    FCM_SEND_MESSAGE(advertisementInd);

    // TODO: Restart timer with settings.advertisementInterval.
}

// ----------------------------------------------------------------------------
// Connect Ok
// ----------------------------------------------------------------------------
void Connector::connectionOk()
{
    // Stop timer.

    FCM_PREPARE_MESSAGE(connectedInd, Transceiving, ConnectedInd);
    connectedInd->connectionId = connectionId;
    FCM_SEND_MESSAGE(connectedInd);

    // TODO: Start timer with settings.connectionTimeout.
}
