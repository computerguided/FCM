// ---------------------------------------------------------------------------------------------------------------------
// Created by Fred Dijkstra on 25/02/2024.
// ---------------------------------------------------------------------------------------------------------------------

#ifndef FCM_FCMDEVICE_H
#define FCM_FCMDEVICE_H

#include <map>

#include <FcmBaseComponent.h>
#include <FcmComponent.h>
#include <FcmAsyncInterfaceHandler.h>
#include <FcmMessage.h>
#include <FcmTimerHandler.h>
#include <FcmMessageQueue.h>


// ---------------------------------------------------------------------------------------------------------------------
// FCM Device
// ---------------------------------------------------------------------------------------------------------------------
class FcmDevice
{
public:
    std::shared_ptr<FcmMessageQueue> mainMessageQueue;

    explicit FcmDevice(int timeStepMsParam);
    virtual void initialize() = 0;
    [[noreturn]] void run();

    template <class ComponentType>
    std::shared_ptr<ComponentType> createComponent(const std::string& name,
                                                              const std::shared_ptr<FcmMessageQueue>& messageQueue,
                                                              const std::map<std::string,std::any>& settings)
    {
        auto component = std::make_shared<ComponentType>(name, messageQueue, timerHandler, settings);
        component->initialize();
        return component;
    }

    std::shared_ptr<FcmMessageQueue> createMessageQueue();
    static void connectInterface(const std::string& interfaceName,
                                 FcmBaseComponent* firstComponent,
                                 FcmBaseComponent* secondComponent);

private:
    std::vector<std::shared_ptr<FcmMessageQueue>> messageQueues;
    std::shared_ptr<FcmTimerHandler> timerHandler;
    const int timeStepMs;

    void processMessages();
    void copyMessages(const std::shared_ptr<FcmMessageQueue>& messageQueue) const;
};


#endif //FCM_FCMDEVICE_H
