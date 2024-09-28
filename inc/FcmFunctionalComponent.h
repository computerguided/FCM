#ifndef FCM_FUNCTIONAL_COMPONENT_H
#define FCM_FUNCTIONAL_COMPONENT_H

// ---------------------------------------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------------------------------------
#include <string>
#include <map>
#include <queue>
#include <any>

#include "FcmBaseComponent.h"
#include "FcmMessage.h"
#include "FcmStateTransitionTable.h"
#include "FcmTimerHandler.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmFunctionalComponent: public FcmBaseComponent
{
public:
    std::string currentState;

    FcmFunctionalComponent(const std::string& nameParam,
                            const std::map<std::string, std::any>& settingsParam = {});

    void initialize();
    void processMessage(const std::shared_ptr<FcmMessage>& message);

protected:
    FcmTimerHandler& timerHandler;
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
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)       \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                      \
    [this](const std::shared_ptr<FcmMessage>& msg)                              \
    {                                                                           \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
        ACTION                                                                  \
    })

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT,                        \
    [this]()                                            \
    {                                                   \
        EVALUATION                                      \
    })

#endif //FCM_FUNCTIONAL_COMPONENT_H