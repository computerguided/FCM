// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 18/02/2024.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef FCM_PROTOTYPING_FCMSTATETRANSITIONTABLE_H
#define FCM_PROTOTYPING_FCMSTATETRANSITIONTABLE_H

#include "FcmMessage.h"

// ---------------------------------------------------------------------------------------------------------------------
// State Transition Table
// ---------------------------------------------------------------------------------------------------------------------

using FcmSttAction = std::function<void(const FcmMessage&)>;

struct FcmSttTransition
{
    FcmSttAction action;
    std::string nextState;
};

using FcmSttMessages = std::map<std::string, FcmSttTransition>;

using FcmSttInterfaces = std::map<std::string, FcmSttMessages>;

using FcmStateTransitionTable = std::map<std::string, FcmSttInterfaces>;

// ----------------------------------------------------------------------------
// Choice Point Table
// ----------------------------------------------------------------------------

using FcmSttEvaluation = std::function<bool()>;
using FcmChoicePointTable = std::map<std::string, FcmSttEvaluation>;


#endif //FCM_PROTOTYPING_FCMSTATETRANSITIONTABLE_H
