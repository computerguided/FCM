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
    explicit FcmDevice(int timeStepMsParam);
    virtual void initialize() = 0;
    [[noreturn]] void run();

    void initializeComponents();

    template <class ComponentType>
    std::shared_ptr<ComponentType> createComponent(const std::string& name,
                                                   const FcmSettings& settings = {})
    {
        auto component = std::make_shared<ComponentType>(name, settings);
        components.push_back(component);
        return component;
    }

protected:
    std::vector<std::shared_ptr<FcmBaseComponent>> components;

    static void connectInterface(const std::string& interfaceName,
                          FcmBaseComponent* firstComponent,
                          FcmBaseComponent* secondComponent);

private:
    FcmMessageQueue& messageQueue;
    FcmTimerHandler& timerHandler;
    const int timeStepMs;

    void processMessages();
};
// ---------------------------------------------------------------------------------------------------------------------
#define FCM_CONNECT_INTERFACE(INTERFACE, FIRST_COMPONENT, SECOND_COMPONENT) \
    connectInterface(#INTERFACE, FIRST_COMPONENT.get(), SECOND_COMPONENT.get())

#endif //FCM_DEVICE_H