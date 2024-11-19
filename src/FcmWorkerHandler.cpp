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
        // Block the main thread until the worker thread has handled the stop request in the run() method.
        workerThread.join();
    }

    // Remove the possible finished message from the message queue
    auto checkFunction = [this](const std::shared_ptr<FcmMessage>& msg) -> bool
    {
        return msg->sender == this;
    };

    messageQueue.removeMessage(finishedMessage->_interfaceName,
                               finishedMessage->_name,
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

    finished();

    // Send the finished message.
    finishedMessage = prepareFinishedMessage();
    sendMessage(finishedMessage);
}
