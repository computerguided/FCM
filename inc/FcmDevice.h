#ifndef FCM_DEVICE_H
#define FCM_DEVICE_H

#include <map>

#include <FcmBaseComponent.h>
#include <FcmFunctionalComponent.h>
#include <FcmAsyncInterfaceHandler.h>
#include <FcmMessage.h>
#include <FcmTimerHandler.h>
#include <FcmMessageQueue.h>

// ---------------------------------------------------------------------------------------------------------------------
class FcmDevice
{
public:
    FcmDevice(int timeStepMsParam);
    virtual void initialize() = 0;
    [[noreturn]] void run();

    void initializeComponents();

    template <class ComponentType, typename... Args>
    std::shared_ptr<ComponentType> createComponent(const std::string& name,
                                                   const std::map<std::string, std::any>& settings = {})
    {
        auto component = std::make_shared<ComponentType>(name, settings);
        components.push_back(component);
        return component;
    }

    static void connectInterface(const std::string& interfaceName,
                                 FcmBaseComponent* firstComponent,
                                 FcmBaseComponent* secondComponent);

private:
    FcmMessageQueue& messageQueue;
    FcmTimerHandler& timerHandler;
    const int timeStepMs;

    std::vector<std::shared_ptr<FcmBaseComponent>> components;

    void processMessages();
};


#endif //FCM_FCMDEVICE_H