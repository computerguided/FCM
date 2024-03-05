//
// Created by Fred Dijkstra on 14/02/2024.
//

#include "../inc/Connector.h"
#include "../inc/Transceiving.h"

// ---------------------------------------------------------------------------------------------------------------------
// Override the setTransitions function to add the transitions.
// ---------------------------------------------------------------------------------------------------------------------
void Connector::setTransitions()
{
    // -----------------------------------------------------------------------------------------------------------------
    // State Transition Table
    // -----------------------------------------------------------------------------------------------------------------
    FCM_ADD_TRANSITION( "Advertising", Timer, Timeout, "Advertising",
    {
        advertise();
    });

    FCM_ADD_TRANSITION( "Advertising", Transceiving, ConnectReq, "Correct Server?",
    {
        timerHandler->cancelTimeout(timerId);
        serverId = message.serverId;
        connectionId = message.connectionId;
    });

    FCM_ADD_TRANSITION( "Correct Server?", Logical, Yes, "Connecting",
    {
        FCM_PREPARE_MESSAGE(connectAck, Transceiving, ConnectAck);
        connectAck->connectionId = connectionId;
        FCM_SEND_MESSAGE(connectAck);

        timerId = timerHandler->setTimeout(settings->connectionTimeout, this);
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
}

// ---------------------------------------------------------------------------------------------------------------------
// Set choice-points
// ---------------------------------------------------------------------------------------------------------------------
void Connector::setChoicePoints()
{
    // -----------------------------------------------------------------------------------------------------------------
    // Choice Point Table
    // -----------------------------------------------------------------------------------------------------------------
    FCM_ADD_CHOICE_POINT( "Correct Server?",
    {
        // Determine whether the server is in the whitelist.
        auto it = std::find(settings->serverWhitelist.begin(),
                            settings->serverWhitelist.end(), serverId);

        return it != settings->serverWhitelist.end();
    });
}

// ---------------------------------------------------------------------------------------------------------------------
// Advertise
// ---------------------------------------------------------------------------------------------------------------------
void Connector::advertise()
{
    FCM_PREPARE_MESSAGE(advertisementInd, Transceiving, AdvertisementInd);
    advertisementInd->clientId = clientId;
    FCM_SEND_MESSAGE(advertisementInd);
    timerId = timerHandler->setTimeout(settings->advertisementInterval, this);
}

// ---------------------------------------------------------------------------------------------------------------------
// Connect Ok
// ---------------------------------------------------------------------------------------------------------------------
void Connector::connectionOk()
{
    timerHandler->cancelTimeout(timerId);

    FCM_PREPARE_MESSAGE(connectedInd, Transceiving, ConnectedInd);
    connectedInd->connectionId = connectionId;
    FCM_SEND_MESSAGE(connectedInd);

    timerId = timerHandler->setTimeout(settings->connectionTimeout, this);
}
