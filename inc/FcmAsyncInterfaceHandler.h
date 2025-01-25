#ifndef FCM_ASYNC_INTERFACE_HANDLER_H
#define FCM_ASYNC_INTERFACE_HANDLER_H

#include "FcmBaseComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmAsyncInterfaceHandler: public FcmBaseComponent
{
public:
    using FcmBaseComponent::FcmBaseComponent;
    virtual void initialize() override = 0; // Override in derived classes if needed.
    FcmComponentType getType() const override { return FcmComponentType::AsyncInterfaceHandler; }
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_ASYNC_INTERFACE_HANDLER(NAME, ...) \
    class NAME : public FcmAsyncInterfaceHandler \
    { \
    public: \
        using FcmAsyncInterfaceHandler::FcmAsyncInterfaceHandler; \
        void initialize() override; \
        __VA_ARGS__ \
    }

#endif //FCM_ASYNC_INTERFACE_HANDLER_H

