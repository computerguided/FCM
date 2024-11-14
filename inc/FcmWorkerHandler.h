#ifndef FCM_WORKER_H
#define FCM_WORKER_H

#include <string>
#include <atomic>
#include <thread>

#include "FcmBaseComponent.h"

// ---------------------------------------------------------------------------------------------------------------------
class FcmWorkerHandler: public FcmBaseComponent
{
public:
    explicit FcmWorkerHandler(const std::string& nameParam,
                       const FcmSettings& settingsParam = {});

    void initialize() override {};

    // Starts the worker thread
    bool start();

    // Cancels the worker thread
    void cancel();

    virtual ~FcmWorkerHandler() override;

protected:
    // Method to be overridden by subclass to perform the long-running task
    virtual void run() = 0;

    // Methods to be overridden by subclass to handle task completion
    virtual std::shared_ptr<FcmMessage> prepareFinishedMessage() = 0;
    virtual void finished() = 0;

private:
    std::thread workerThread; // Empty thread object (not yet running)
    std::atomic<bool> cancelRequested{false};

    // Internal thread function that runs the worker and calls finished()
    void threadRun();

    // Placeholder for the finished message
    std::shared_ptr<FcmMessage> finishedMessage;
};

#endif //FCM_WORKER_H

