#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class DSPThreadPool {
public:
    DSPThreadPool();
    ~DSPThreadPool();

    // Initializes the instance
    void initialize(size_t numThreads);

    // Submit a task to the pool
    void execute(std::function<void()> task);

    // Wait until all submitted tasks are completed
    void wait();

    // Prevent copy
    DSPThreadPool(const DSPThreadPool&) = delete;
    DSPThreadPool& operator=(const DSPThreadPool&) = delete;

private:
    // Worker loop for each thread
    void workerThread();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex taskMutex;
    std::condition_variable taskAvailable;

    std::atomic<int> activeTasks;
    std::mutex waitMutex;
    std::condition_variable allTasksDone;

    bool shuttingDown;
};
