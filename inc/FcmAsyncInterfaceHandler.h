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
    FcmAsyncInterfaceHandler(const std::string& nameParam,
                             const std::map<std::string, std::any>& settingsParam = {});

    virtual void initialize() = 0;
};

#endif //FCM_ASYNC_INTERFACE_HANDLER_H

