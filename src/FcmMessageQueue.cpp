#include <optional>
#include <memory>

#include "FcmMessage.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
void FcmMessageQueue::push(const std::shared_ptr<FcmMessage>& message)
{
    std::lock_guard<std::mutex> lock(mutex);
    
    message->timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

    queue.push_back(message);
    conditionVariable.notify_one();
}

// ---------------------------------------------------------------------------------------------------------------------
std::shared_ptr<FcmMessage> FcmMessageQueue::awaitMessage()
{
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock, [this]() { return !queue.empty(); });
    auto message = std::move(queue.front());
    queue.pop_front();
    return message;
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmMessageQueue::removeMessage(const std::string& interfaceName,
                                    const std::string& messageName,
                                    const FcmMessageCheckFunction& checkFunction)
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto it = queue.begin(); it != queue.end(); ++it)
    {
        const auto& message = *it;
        if (message->_interfaceName == interfaceName && message->_name == messageName)
        {
            if (checkFunction && !checkFunction(message)) {continue;}
            queue.erase(it);
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmMessageQueue::resendMessage(const std::shared_ptr<FcmMessage>& message)
{
    std::lock_guard<std::mutex> lock(mutex);
    queue.push_front(message);
}


