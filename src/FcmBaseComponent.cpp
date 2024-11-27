#include <utility>

#include "FcmBaseComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmBaseComponent::FcmBaseComponent(std::string nameParam,
                                   const FcmSettings& settingsParam):
    name(std::move(nameParam)),
    settings(settingsParam)
{
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::connectInterface(const std::string& interfaceName,
                                        FcmBaseComponent *remoteComponent)
{
    auto& componentList = interfaces[interfaceName];

    if (std::find(componentList.begin(), componentList.end(), remoteComponent) != componentList.end())
    {
        logError("Interface \"" + interfaceName +
                 "\" is already connected to component \"" +
                 remoteComponent->name + "\"!");
        return;
    }

    componentList.push_back(remoteComponent);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::sendMessage(const std::shared_ptr<FcmMessage>& message, size_t index)
{
    auto interfaceIt = interfaces.find(message->_interfaceName);
    if (interfaceIt == interfaces.end())
    {
        logError("Trying to send message \"" + message->_name +
                 "\" to interface \"" + message->_interfaceName + "\" but the interface is not connected!");
        return;
    }

    auto& componentList = interfaceIt->second;
    if (index >= componentList.size())
    {
        logError("Trying to send message \"" + message->_name +
                 "\" to interface \"" + message->_interfaceName + "\" on index " +
                 std::to_string(index) + " but there are only " +
                 std::to_string(componentList.size()) + " components connected!");
        return;
    }

    message->receiver = componentList[index];
    message->interfaceIndex = index;
    messageQueue.push(message);
}

// ---------------------------------------------------------------------------------------------------------------------
[[maybe_unused]] void FcmBaseComponent::logError(const std::string& message)
{
    if (logErrorFunction.has_value())
    {
        logErrorFunction.value()(getLogPrefix("ERROR") + message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
[[maybe_unused]] void FcmBaseComponent::logWarning(const std::string& message)
{
    if (logWarningFunction.has_value())
    {
        logWarningFunction.value()(getLogPrefix("WARNING") + message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
[[maybe_unused]] void FcmBaseComponent::logInfo(const std::string& message)
{
    if (logInfoFunction.has_value())
    {
        logInfoFunction.value()(getLogPrefix("INFO") + message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
[[maybe_unused]] void FcmBaseComponent::logDebug(const std::string &message)
{
    if (logDebugFunction.has_value())
    {
        logDebugFunction.value()(getLogPrefix("DEBUG") + message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
[[maybe_unused]] void FcmBaseComponent::fatalError(const std::string& message)
{
    if (fatalErrorFunction.has_value())
    {
        fatalErrorFunction.value()(getLogPrefix("FATAL ERROR") + message);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
std::string FcmBaseComponent::getLogPrefix(const std::string& logLevel) const
{
    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert to local time struct
    std::tm local_tm = *std::localtime(&now_time_t);

    // Format the time string
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);
    std::string timeString(buffer);

    return timeString + " - " + logLevel + " - " + name + " - ";
}