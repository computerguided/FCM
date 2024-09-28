#ifndef FCM_MESSAGE_QUEUE_H
#define FCM_MESSAGE_QUEUE_H

#include <list>
#include <mutex>
#include <memory>
#include <optional>
#include <functional>

#include <FcmMessage.h>

using FcmMessageCheckFunction = std::function<bool(const std::shared_ptr<FcmMessage>&)>;

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessageQueue
{
private:
    std::list<std::shared_ptr<FcmMessage>> queue;
    std::mutex mutex;

    FcmMessageQueue() = default;
    FcmMessageQueue(const FcmMessageQueue&) = delete;
    FcmMessageQueue& operator=(const FcmMessageQueue&) = delete;

public:

    static FcmMessageQueue& getInstance()
    {
        static FcmMessageQueue instance;
        return instance;
    }
    
    void push(const std::shared_ptr<FcmMessage>& message);
    std::optional<std::shared_ptr<FcmMessage>> pop();
    void removeMessage(const std::string& interfaceName,
                       const std::string& messageName,
                       const FcmMessageCheckFunction& checkFunction);
};


#endif //FCM_MESSAGE_QUEUE_H