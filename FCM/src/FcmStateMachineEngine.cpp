//
// Created by Fred Dijkstra on 17/02/2024.
//

#include "FcmStateMachineEngine.h"

// ---------------------------------------------------------------------------------------------------------------------
// Handle message
// ---------------------------------------------------------------------------------------------------------------------
void FcmStateMachineEngine::handleMessage(const FcmMessage& message)
{
    // Get the receiver of the message.
    auto receiver = (FcmComponent*)message.receiver;

    if (receiver == nullptr)
    {

        // TODO: Log error.
        return;
    }

    // Process the message.
    receiver->processMessage(message);

    // Loop until the new state of the receiver is not a choice-point.
    while (receiver->atChoicePoint())
    {
        // Run the evaluation function of the choice-point.
        bool result = receiver->evaluateChoicePoint(receiver->currentState);

        // Process the result of the choice-point.
        std::shared_ptr<FcmMessage> choicePointMessage;
        if (result)
        {
            choicePointMessage = std::make_shared<Logical::Yes>();
        }
        else
        {
            choicePointMessage = std::make_shared<Logical::No>();
        }
        choicePointMessage->interfaceName = "Logical";
        receiver->processMessage(*choicePointMessage);
    }
}
