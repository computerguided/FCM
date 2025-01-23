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

    // -----------------------------------------------------------------------------------------------------------------
    template<typename MessageType, typename Action>
    inline void addTransitionFunction(const std::string& state, const std::string& nextState, Action action)
    {
        addTransition(state, MessageType::interfaceName, MessageType::name, nextState,
        [action](const std::shared_ptr<FcmMessage>& msg)
        {
            const auto& message = static_cast<const MessageType&>(*msg);
            action(message);
        });
    }

    // -----------------------------------------------------------------------------------------------------------------
    template<typename MessageType, typename Action>
    inline void addMultipleStatesTransition(const std::vector<std::string>& states, const std::string& nextState, Action action)
    {
        for (const auto& state : states)
        {
            addTransition(state, MessageType::interfaceName, MessageType::name, nextState,
            [action](const std::shared_ptr<FcmMessage>& msg)
            {
                const auto& message = static_cast<const MessageType&>(*msg);
                action(message);
            });
        }
    }

    // -----------------------------------------------------------------------------------------------------------------
    template<typename MessageType>
    inline std::shared_ptr<MessageType> castLastReceivedMessage()
    {
        auto message = std::dynamic_pointer_cast<MessageType>(lastReceivedMessage);
        if (message == nullptr)
        {
            throw std::runtime_error("Last received message cast to invalid message type \"" + MessageType::interfaceName + ":" + MessageType::name + "\"!");
        }
        return message;
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

#endif //FCM_FUNCTIONAL_COMPONENT_H