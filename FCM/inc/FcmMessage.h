// --------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// --------------------------------------------------------------------------------------------------------------------

#ifndef FCM_PROTOTYPING_FCMMESSAGE_H
#define FCM_PROTOTYPING_FCMMESSAGE_H

#include <string>
#include <cstdint>
#include <queue>

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessage
{
public:
    void* sender; // Pointer to the sender of the message.
    void* receiver; // Pointer to the receiver of the message.
    uint32_t timestamp; // Timestamp in milliseconds at which the message was sent.
    std::string interfaceName; // Name of the interface through which the message was sent.
    std::string name; // Name of the message.
};

// ---------------------------------------------------------------------------------------------------------------------
// Message Queue Type
// ---------------------------------------------------------------------------------------------------------------------
using FcmMessageQueue = std::queue<std::shared_ptr<FcmMessage>>;

// ---------------------------------------------------------------------------------------------------------------------
// Define a message inside an interface
// ---------------------------------------------------------------------------------------------------------------------
#define FCM_DEFINE_MESSAGE( NAME, PARAMETERS ) \
class NAME : public FcmMessage                  \
{                                               \
public:                                         \
    PARAMETERS                                  \
    NAME() { name = #NAME; interfaceName = FCM_INTERFACE_NAME; }                    \
}

#endif //FCM_PROTOTYPING_FCMMESSAGE_H
