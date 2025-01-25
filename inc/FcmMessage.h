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
    int   interfaceIndex = 0;
    int64_t timestamp{};
    std::string getInterfaceName() const { return interfaceName; }
    std::string getName() const { return name; }
    void setInterfaceName(const std::string& newInterfaceName) { interfaceName = newInterfaceName; }
    void setName(const std::string& newName) { name = newName; }

    virtual ~FcmMessage() = default;
private:
    std::string interfaceName;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_DEFINE_MESSAGE(NAME, ...)                                    \
    class NAME : public FcmMessage                                       \
    {                                                                    \
    public:                                                              \
        __VA_ARGS__                                                      \
        static constexpr const char* interfaceName = interfaceClassName; \
        static constexpr const char* name = #NAME;                       \
        NAME() { setInterfaceName(interfaceClassName); setName(#NAME); }   \
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

