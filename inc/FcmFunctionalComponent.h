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
    std::string currentState = "NONE";
    std::string historyState = "NONE";

    explicit FcmFunctionalComponent(const std::string& nameParam,
                                    const FcmSettings& settingsParam = {});

    void initialize() override {}; // Override in derived classes if needed.
    void processMessage(const std::shared_ptr<FcmMessage>& message);

    template<typename MessageType, typename Action>
    inline void addTransitionFunction(const std::string& state, const std::string& nextState, Action action)
    {
        addTransition(state, MessageType::interfaceName, MessageType::name, nextState,
        [action](const std::shared_ptr<FcmMessage>& msg)
        {
            const auto& message = dynamic_cast<const MessageType&>(*msg);
            action(message);
        });
    }

    virtual void _initialize() override;

protected:
    FcmTimerHandler& timerHandler = FcmTimerHandler::getInstance();
    FcmStateTransitionTable stateTransitionTable;
    FcmChoicePointTable choicePointTable;
    std::shared_ptr<FcmMessage> lastReceivedMessage;

    std::vector<std::string> states;

    virtual void setTransitions() = 0;
    virtual void setChoicePoints() = 0;
    virtual void setStates() = 0;

    void addTransition(const std::string& stateName,
                       const std::string& interfaceName,
                       const std::string& messageName,
                       const std::string& nextState,
                       const FcmSttAction& action);

    void addChoicePoint( const std::string& choicePointName,
                         const FcmSttEvaluation& evaluationFunction);

    bool performTransition(const std::shared_ptr<FcmMessage>& message);

    [[nodiscard]] bool evaluateChoicePoint(const std::string& choicePointName) const;
    void resendLastReceivedMessage();

    std::optional<FcmSttTransition> getTransition(const std::string& stateName,
                                                  const std::string& interfaceName,
                                                  const std::string& messageName,
                                                        std::string* notFoundReason = nullptr) const;

    [[nodiscard]] int setTimeout(FcmTime timeout);
    void cancelTimeout(int timerId);
};

// ---------------------------------------------------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_FUNCTIONAL_COMPONENT(NAME, ...) \
    class NAME : public FcmFunctionalComponent \
    { \
    public: \
        using FcmFunctionalComponent::FcmFunctionalComponent; \
        void initialize() override; \
    protected: \
        void setStates() override; \
        void setTransitions() override; \
        void setChoicePoints() override; \
    private: \
        __VA_ARGS__ \
    }

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_ADD_TRANSITION(STATE, INTERFACE, MESSAGE, NEXT_STATE, ACTION)       \
    addTransition(STATE, #INTERFACE, #MESSAGE, NEXT_STATE,                      \
    [this](const std::shared_ptr<FcmMessage>& msg)                              \
    {                                                                           \
        const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
        (void)message;                                                          \
        (void)this;                                                             \
        ACTION                                                                  \
    })

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_ADD_MULTIPLE_STATES_TRANSITION(STATES, INTERFACE, MESSAGE, NEXT_STATE, ACTION)  \
    do                                                                                      \
    {                                                                                       \
        for (const auto& state : std::vector<std::string>STATES)                            \
        {                                                                                   \
            addTransition(state, #INTERFACE, #MESSAGE, NEXT_STATE,                          \
                [this](const std::shared_ptr<FcmMessage>& msg)                              \
                {                                                                           \
                    const auto& message = dynamic_cast<const INTERFACE::MESSAGE&>(*msg);    \
                    ACTION                                                                  \
                });                                                                         \
        }                                                                                   \
    } while (0)

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_ADD_CHOICE_POINT(CHOICE_POINT, EVALUATION)  \
    addChoicePoint(CHOICE_POINT,                        \
    [this]()                                            \
    EVALUATION                                          \
    )

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_CAST_LAST_RECEIVED_MESSAGE(MESSAGE, INTERFACE, MESSAGE_TYPE) \
    const auto& MESSAGE = std::dynamic_pointer_cast<INTERFACE::MESSAGE_TYPE>(lastReceivedMessage); \
    if (MESSAGE == nullptr)                                              \
    { \
        throw std::runtime_error("Last received message cast to invalid message type \""#INTERFACE":"#MESSAGE_TYPE"\"!"); \
    }

#endif //FCM_FUNCTIONAL_COMPONENT_H