#ifndef FCM_WORKER_H
#define FCM_WORKER_H

#include <string>
#include <atomic>
#include <thread>

#include "FcmAsyncInterfaceHandler.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmWorkerHandler: public FcmAsyncInterfaceHandler
{
public:
    using FcmAsyncInterfaceHandler::FcmAsyncInterfaceHandler;

    void initialize() override {}; // Override in derived classes if needed.

    // Starts the worker thread
    bool start();

    // Cancels the worker thread
    void cancel();

    virtual ~FcmWorkerHandler() override;

protected:
    // Method to be overridden by subclass to perform the long-running task
    virtual void run() = 0;

    // Method to be overridden by subclass to prepare the finished message
    virtual std::shared_ptr<FcmMessage> prepareFinishedMessage() = 0;

private:
    std::thread workerThread; // Empty thread object (not yet running)
    std::atomic<bool> cancelRequested{false};

    // Internal thread function that runs the worker
    void threadRun();

    // Placeholder for the finished message
    std::shared_ptr<FcmMessage> finishedMessage;
};

#endif //FCM_WORKER_H

