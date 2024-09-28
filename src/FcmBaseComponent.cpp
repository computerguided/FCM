#include <utility>

#include "FcmBaseComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmBaseComponent::FcmBaseComponent(std::string nameParam,
                                   const FcmSettings& settingsParam):
    name(std::move(nameParam)), 
    messageQueue(FcmMessageQueue::getInstance()), 
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
        throw std::runtime_error("Interface \"" + interfaceName +
                                 "\" of component \"" + name + "\" is already connected to component \"" +
                                 remoteComponent->name + "\"!");
    }

    componentList.push_back(remoteComponent);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmBaseComponent::sendMessage(const std::shared_ptr<FcmMessage>& message, size_t index)
{
    try
    {
        auto& componentList = interfaces.at(message->interfaceName);
        if (index >= componentList.size())
        {
            throw std::out_of_range("Component \"" + name + "\" tries to send message \"" + message->name +
                                    "\" to interface \"" + message->interfaceName + "\" on index " +
                                    std::to_string(index) + " but there are only " +
                                    std::to_string(componentList.size()) + " components connected!");
        }
        message->receiver = componentList[index];
    }
    catch (const std::out_of_range& e)
    {
        message->receiver = nullptr;
    }

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
std::string FcmBaseComponent::getLogPrefix(const std::string& logLevel) const
{
    // Get the current printable time
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string timeString = std::ctime(&now_c);

    // Remove the newline character from the end of the string
    timeString.pop_back();

    return timeString + " - " + logLevel + " - " + name + " - ";
}