// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// Documentation: https://github.com/computerguided/FCM/blob/main/FCM/doc/AsyncInterfaceHandler.md
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmAsyncInterfaceHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmAsyncInterfaceHandler::FcmAsyncInterfaceHandler(const std::string& nameParam,
                                                   const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                                                   const std::map<std::string, std::any>& settingsParam):
                                                   FcmBaseComponent(nameParam, messageQueueParam, settingsParam)
{
}

