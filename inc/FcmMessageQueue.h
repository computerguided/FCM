#ifndef FCM_MESSAGE_QUEUE_H
#define FCM_MESSAGE_QUEUE_H

#include <list>
#include <mutex>
#include <memory>
#include <optional>
#include <functional>
#include <condition_variable>

#include <FcmMessage.h>

using FcmMessageCheckFunction = std::function<bool(const std::shared_ptr<FcmMessage>&)>;

// ---------------------------------------------------------------------------------------------------------------------
class FcmMessageQueue
{
private:
    std::list<std::shared_ptr<FcmMessage>> queue;
    std::mutex mutex;
    std::condition_variable conditionVariable;

public:
    FcmMessageQueue() = default;
    FcmMessageQueue(const FcmMessageQueue&) = delete;
    FcmMessageQueue& operator=(const FcmMessageQueue&) = delete;

    static FcmMessageQueue& getInstance()
    {
        static FcmMessageQueue instance;
        return instance;
    }
    
    void push(const std::shared_ptr<FcmMessage>& message);
    std::shared_ptr<FcmMessage> awaitMessage();
    bool removeMessage(const std::string& interfaceName,
                       const std::string& messageName,
                       const FcmMessageCheckFunction& checkFunction);
    void resendMessage( const std::shared_ptr<FcmMessage>& message);
};


#endif //FCM_MESSAGE_QUEUE_H