#ifndef FCM_BASE_COMPONENT_H
#define FCM_BASE_COMPONENT_H

#include <string>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include <optional>

#include "FcmMessage.h"
#include "FcmMessageQueue.h"

using FcmSettings = std::map<std::string, std::any>;

// Optional log function type
using FcmLogFunction = std::optional<std::function<void(const std::string& message)>>;

// ---------------------------------------------------------------------------------------------------------------------
class FcmBaseComponent
{
public:
    const std::string name;
    const FcmSettings& settings;

    // Log functions
    FcmLogFunction logInfoFunction;
    FcmLogFunction logWarningFunction;
    FcmLogFunction logErrorFunction;
    FcmLogFunction logDebugFunction;
    FcmLogFunction logTransitionFunction;

    explicit FcmBaseComponent(std::string nameParam,
                             const FcmSettings& settingsParam = {});

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
            logError("Settings error for '" + settingName + "' : cast error");
        }
        catch (const std::out_of_range& e)
        {
            logError("Settings error for '" + settingName + "' : key not found");
        }
    }

    virtual ~FcmBaseComponent() = default;
    virtual void initialize() = 0;
    virtual void _initialize()
    {
        initialize();
    }

    // Logging
    [[maybe_unused]] void logError(const std::string& message);
    [[maybe_unused]] void logWarning(const std::string& message);
    [[maybe_unused]] void logInfo(const std::string& message);
    [[maybe_unused]] void logDebug(const std::string& message);

protected:
    std::map<std::string, std::vector<FcmBaseComponent*>> interfaces;
    FcmMessageQueue& messageQueue;

    [[nodiscard]] std::string getLogPrefix(const std::string& logLevel) const;
};

// ---------------------------------------------------------------------------------------------------------------------
#define FCM_SEND_MESSAGE(MESSAGE, ...) \
    sendMessage(MESSAGE, ##__VA_ARGS__)

#endif // FCM_BASE_COMPONENT_H