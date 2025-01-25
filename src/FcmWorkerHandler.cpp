#include <thread>

#include "FcmWorkerHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
FcmWorkerHandler::~FcmWorkerHandler()
{
    cancel();
}

// ---------------------------------------------------------------------------------------------------------------------
bool FcmWorkerHandler::start()
{
    if (workerThread.joinable())
    {
        logError("Worker: " + name + " already started");
        return false;
    }

    cancelRequested = false;
    workerThread = std::thread(&FcmWorkerHandler::threadRun, this);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmWorkerHandler::cancel()
{
    cancelRequested = true;

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    auto checkFunction = [this](const std::shared_ptr<FcmMessage>& msg) -> bool
    {
        return msg->sender == this;
    };

    messageQueue.removeMessage(finishedMessage->getInterfaceName(),
                               finishedMessage->getName(),
                               checkFunction);
}

// ---------------------------------------------------------------------------------------------------------------------
void FcmWorkerHandler::threadRun()
{
    run();

    if (cancelRequested)
    {
        return;
    }

    finishedMessage = prepareFinishedMessage();
    sendMessage(finishedMessage);
}
