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

#include "FcmMessage.h"
#include "FcmTimerInterface.h"
#include "FcmLogicalInterface.h"
#include "FcmStateTransitionTable.h"
#include "FcmTimerHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------------------------------------------------
class FcmComponent;

// ---------------------------------------------------------------------------------------------------------------------
// FCM Component
// ---------------------------------------------------------------------------------------------------------------------

class FcmComponent
{
public:
    const std::string name;
    std::string currentState;

    FcmStateTransitionTable stateTransitionTable;
    FcmChoicePointTable choicePointTable;
    std::map<std::string, FcmComponent*> interfaces;

    FcmComponent(std::string& nameParam,
                 const std::shared_ptr<FcmMessageQueue>& messageQueueParam,
                 const std::shared_ptr<FcmTimerHandler>& timerHandlerParam);

    void initialize();

    virtual void setTransitions(){}; // This function is to be overridden by the user.
    virtual void setChoicePoints(){}; // This function is to be overridden by the user.

    virtual void setTimerHandler( const std::shared_ptr<FcmTimerHandler>& timerHandlerParam )
    {
        timerHandler = timerHandlerParam;
    }

    void addTransition(const std::string& stateName,
                       const std::string& interfaceName,
                       const std::string& messageName,
                       const std::string& nextState,
                       const FcmSttAction& action);

    void addChoicePoint( const std::string& choicePointName,
                         const FcmSttEvaluation& evaluationFunction);

    [[nodiscard]] bool atChoicePoint() const;

    void sendMessage(const std::shared_ptr<FcmMessage>& message);

    void processMessage(const FcmMessage& message);

    [[nodiscard]] bool evaluateChoicePoint(const std::string& choicePointName) const;

    void connectInterface(const std::string& interfaceName, FcmComponent* remoteComponent);

    const std::shared_ptr<FcmMessageQueue> messageQueue;
    std::shared_ptr<FcmTimerHandler> timerHandler;

};

// ---------------------------------------------------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------------------------------------------------

// Convenience macro for adding a transition to the state transition table.
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)   \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                  \
    [this](const FcmMessage& msg)                                           \
    {                                                                       \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(msg);  \
        ACTION                                                              \
    })

// Convenience macro for adding a choice point to the choice point table.
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)                      \
    addChoicePoint(CHOICE_POINT,                                            \
    [this]()                                                                \
    {                                                                       \
        \
        EVALUATION                                                          \
    })

// ---------------------------------------------------------------------------------------------------------------------
// Prepare a message with parameters.
// ---------------------------------------------------------------------------------------------------------------------
#define FCM_PREPARE_MESSAGE( MESSAGE, INTERFACE, MESSAGE_TYPE ) \
    auto MESSAGE = std::make_shared<INTERFACE::MESSAGE_TYPE>()

// ---------------------------------------------------------------------------------------------------------------------
// Send a message.
// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SEND_MESSAGE( MESSAGE ) \
    sendMessage(MESSAGE)

// ---------------------------------------------------------------------------------------------------------------------
// Connect the interface of two components.
// ---------------------------------------------------------------------------------------------------------------------
#define FCM_CONNECT_INTERFACE( INTERFACE, COMPONENT_1, COMPONENT_2 ) \
    COMPONENT_1->connectInterface(#INTERFACE, COMPONENT_2)           \
    COMPONENT_2->connectInterface(#INTERFACE, COMPONENT_1)

#endif //FCM_PROTOTYPING_FCMCOMPONENT_H
