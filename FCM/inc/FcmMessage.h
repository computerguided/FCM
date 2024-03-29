// --------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// --------------------------------------------------------------------------------------------------------------------

#ifndef FCM_PROTOTYPING_FCMMESSAGE_H
#define FCM_PROTOTYPING_FCMMESSAGE_H

#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessage
{
public:
    void* receiver = nullptr; // Pointer to the receiver of the message.
    uint32_t timestamp; // Timestamp in milliseconds at which the message was sent.
    std::string interfaceName; // Name of the interface through which the message was sent.
    std::string name; // Name of the message.

    virtual ~FcmMessage() = default;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_DEFINE_MESSAGE(NAME, ...)                               \
    class NAME : public FcmMessage                                  \
    {                                                               \
    public:                                                         \
        __VA_ARGS__                                                 \
        NAME() { name = #NAME; interfaceName = currentNamespace; }  \
    }

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_PREPARE_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE) \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>()

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SET_INTERFACE(NAME, ...)                \
    namespace NAME                                  \
    {                                               \
        const char* const currentNamespace = #NAME; \
        __VA_ARGS__                                 \
    }

#endif //FCM_PROTOTYPING_FCMMESSAGE_H
