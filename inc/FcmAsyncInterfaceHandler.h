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
    explicit FcmAsyncInterfaceHandler(const std::string& nameParam,
                                      const FcmSettings& settingsParam = {});

    void initialize() override {};
};

#endif //FCM_ASYNC_INTERFACE_HANDLER_H

