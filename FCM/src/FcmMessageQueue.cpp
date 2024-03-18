// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 05/03/2024.
// Documentation: https://github.com/computerguided/FCM/blob/main/FCM/doc/MessageQueue.md
// ---------------------------------------------------------------------------------------------------------------------

#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
void FcmMessageQueue::push(const std::shared_ptr<FcmMessage>& message)
{
    message->timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

    queue.push_back(message);
}

// ---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<FcmMessage> FcmMessageQueue::pop()
{
    std::shared_ptr<FcmMessage> message = queue.front();
    queue.pop_front();
    return message;
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmMessageQueue::empty() const
{
    return queue.empty();
}
