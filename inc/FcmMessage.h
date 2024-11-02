#ifndef FCM_MESSAGE_H
#define FCM_MESSAGE_H

#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------
class FcmInterface
{
protected:
    FcmInterface() = default; // Protected constructor
};

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessage
{
public:
    void* receiver = nullptr;
    void* sender = nullptr;
    int64_t timestamp{};
    std::string _interfaceName;
    std::string _name;

    virtual ~FcmMessage() = default;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_PREPARE_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE)   \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>(); \
    MESSAGE->sender = this

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_DEFINE_MESSAGE(NAME, ...)                                    \
    class NAME : public FcmMessage                                       \
    {                                                                    \
    public:                                                              \
        __VA_ARGS__                                                      \
        static constexpr const char* interfaceName = interfaceClassName; \
        static constexpr const char* name = #NAME;                       \
        NAME() { _interfaceName = interfaceName; _name = name; }         \
    }

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SET_INTERFACE(NAME, ...)                             \
    class NAME : public FcmInterface                             \
    {                                                            \
    public:                                                      \
        static constexpr const char* interfaceClassName = #NAME; \
        __VA_ARGS__                                              \
    }

// ---------------------------------------------------------------------------------------------------------------------
FCM_SET_INTERFACE( Logical,
    FCM_DEFINE_MESSAGE( Yes );
    FCM_DEFINE_MESSAGE( No );
);

#endif //FCM_MESSAGE_H

