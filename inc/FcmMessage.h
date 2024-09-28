#ifndef FCM_MESSAGE_H
#define FCM_MESSAGE_H

#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessage
{
public:
    void* receiver = nullptr;
    int64_t timestamp;
    std::string interfaceName;
    std::string name;

    virtual ~FcmMessage() = default;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_PREPARE_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE) \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>()

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_DEFINE_MESSAGE(NAME, ...)                               \
    class NAME : public FcmMessage                                  \
    {                                                               \
    public:                                                         \
        __VA_ARGS__                                                 \
        NAME() { name = #NAME; interfaceName = currentNamespace; }  \
    }

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SET_INTERFACE(NAME, ...)                \
    namespace NAME                                  \
    {                                               \
        const char* const currentNamespace = #NAME; \
        __VA_ARGS__                                 \
    }

// ---------------------------------------------------------------------------------------------------------------------
FCM_SET_INTERFACE( Logical,
    FCM_DEFINE_MESSAGE( Yes );
    FCM_DEFINE_MESSAGE( No );
);

#endif //FCM_MESSAGE_H

