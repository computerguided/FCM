#ifndef FCM_ASYNC_INTERFACE_HANDLER_H
#define FCM_ASYNC_INTERFACE_HANDLER_H

#include <string>

#include "FcmBaseComponent.h"
#include "FcmMessage.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmAsyncInterfaceHandler: public FcmBaseComponent
{
public:
    using FcmBaseComponent::FcmBaseComponent;

    virtual void initialize() override {}; // Override in derived classes if needed.
};
    
#endif //FCM_ASYNC_INTERFACE_HANDLER_H

