#ifndef FCM_BASE_COMPONENT_H
#define FCM_BASE_COMPONENT_H

#include <string>
#include <map>
#include <any>

#include "FcmMessage.h"
#include "FcmMessageQueue.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmBaseComponent
{
public:
    const std::string name;
    const std::map<std::string, std::any> settings;

    FcmBaseComponent(std::string nameParam,
                     const std::map<std::string, std::any>& settingsParam = {});

    void connectInterface(const std::string& interfaceName, FcmBaseComponent* remoteComponent);

    void sendMessage(const std::shared_ptr<FcmMessage>& message, size_t index = 0);

    template <typename T>
    void setSetting(const std::string& settingName, T& stateVariable)
    {
        try
        {
            stateVariable = std::any_cast<T>(settings.at(settingName));
        }
        catch (const std::bad_any_cast& e)
        {
            throw std::runtime_error("Component: " + name + " settings error: " + e.what());
        }
    }

    virtual ~FcmBaseComponent() = default;

protected:
    std::map<std::string, std::vector<FcmBaseComponent*>> interfaces;
    FcmMessageQueue& messageQueue;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SEND_MESSAGE(MESSAGE, ...) \
    sendMessage(MESSAGE, ##__VA_ARGS__)

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SET_SETTING(SETTING) \
    setSetting(#SETTING, SETTING)

#endif // FCM_BASE_COMPONENT_H