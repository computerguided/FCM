// ----------------------------------------------------------------------------
// Created by Fred Dijkstra on 22/01/2024.
// ----------------------------------------------------------------------------

#ifndef FCM_PROTOTYPING_FCMCOMPONENT_H
#define FCM_PROTOTYPING_FCMCOMPONENT_H

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include <string>
#include <map>
#include <queue>
#include <any>

#include "FcmBaseComponent.h"
#include "FcmMessage.h"
#include "FcmTimerInterface.h"
#include "FcmLogicalInterface.h"
#include "FcmStateTransitionTable.h"
#include "FcmTimerHandler.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
// FCM Component
// ---------------------------------------------------------------------------------------------------------------------

class FcmComponent: public FcmBaseComponent
{
public:
    std::string currentState;

    FcmComponent(const std::string& nameParam,
                 const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                 const std::shared_ptr<FcmTimerHandler>& timerHandlerParam,
                 const std::map<std::string, std::any>& settingsParam);

    void initialize();
    void processMessage(const std::shared_ptr<FcmMessage>& message);

protected:
    std::shared_ptr<FcmTimerHandler> timerHandler;
    FcmStateTransitionTable stateTransitionTable;
    FcmChoicePointTable choicePointTable;

    virtual void setTransitions() = 0;
    virtual void setChoicePoints() = 0;

    void addTransition(const std::string& stateName,
                       const std::string& interfaceName,
                       const std::string& messageName,
                       const std::string& nextState,
                       const FcmSttAction& action);

    void addChoicePoint( const std::string& choicePointName,
                         const FcmSttEvaluation& evaluationFunction);

    void performTransition(const std::shared_ptr<FcmMessage>& message);

    [[nodiscard]] bool evaluateChoicePoint(const std::string& choicePointName) const;
};

// ---------------------------------------------------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------------------------------------------------

// Convenience macro for adding a transition to the state transition table.
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)       \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                      \
    [this](const std::shared_ptr<FcmMessage>& msg)                              \
    {                                                                           \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
        ACTION                                                                  \
    })

// Convenience macro for adding a choice point to the choice point table.
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT,                        \
    [this]()                                            \
    {                                                   \
        EVALUATION                                      \
    })

#endif //FCM_PROTOTYPING_FCMCOMPONENT_H
