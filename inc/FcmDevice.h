#ifndef FCM_DEVICE_H
#define FCM_DEVICE_H

#include <map>
#include <optional>

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
    explicit FcmDevice();
    virtual void initialize() = 0;
    [[noreturn]] void run();

protected:
    FcmSettings settings{};
    std::vector<std::shared_ptr<FcmBaseComponent>> components;

    // -----------------------------------------------------------------------------------------------------------------
    template <class Interface>
    static void connectInterface(std::shared_ptr<FcmBaseComponent> firstComponent,
                                 std::shared_ptr<FcmBaseComponent> secondComponent)
    {
        _connectInterface(Interface::interfaceClassName, firstComponent.get(), secondComponent.get());
    }

    // -----------------------------------------------------------------------------------------------------------------
    static void _connectInterface(const std::string& interfaceName,
                                  FcmBaseComponent* firstComponent,
                                  FcmBaseComponent* secondComponent);

    void initializeComponents();

    template <class ComponentType>
    std::shared_ptr<ComponentType> createComponent(const std::string& _name,
                                                   const FcmSettings& _settings)
    {
        auto component = std::make_shared<ComponentType>(_name, _settings);
        components.push_back(component);

        // Add the component to the settings if it is a Asynchronous Interface Handler
        if (dynamic_cast<FcmAsyncInterfaceHandler*>(component.get()) != nullptr)
        {
            settings[_name] = std::any(component);
        }

        return component;
    }

private:
    FcmMessageQueue& messageQueue;
    void processMessages(std::shared_ptr<FcmMessage>& message);
};

#endif //FCM_DEVICE_H