//
// Created by Fred Dijkstra on 05/03/2024.
//

#ifndef FCM_FCMMESSAGEQUEUE_H
#define FCM_FCMMESSAGEQUEUE_H

#include <list>
#include <FcmMessage.h>

class FcmMessageQueue
{
public:
    using iterator = std::list<std::shared_ptr<FcmMessage>>::iterator;

    void push(const std::shared_ptr<FcmMessage>& message);
    [[nodiscard]] bool empty() const;
    std::shared_ptr<FcmMessage> pop();

    // Wrapper functions for std::list iterators
    iterator erase(iterator position) { return queue.erase(position); }
    iterator begin() { return queue.begin(); }
    iterator end() { return queue.end(); }

private:
    std::list<std::shared_ptr<FcmMessage>> queue;
};


#endif //FCM_FCMMESSAGEQUEUE_H
