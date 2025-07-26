#include "DSPThreadPool.h"
#include "clamp.h"

DSPThreadPool::DSPThreadPool() : activeTasks(0)
{
#ifdef __linux__
#include <pthread.h>
    pthread_setname_np(pthread_self(), "DSPThreadPool");
#endif
}

DSPThreadPool::~DSPThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        shuttingDown = true;
    }

    taskAvailable.notify_all();

    for (auto &thread : workers)
    {
        if (thread.joinable())
            thread.join();
    }
}

void DSPThreadPool::initialize(size_t numThreads)
{
    size_t threads = clampmin(numThreads, static_cast<size_t>(2));

    // Ensure all tasks are completed before reinitializing
    wait();

    // Shut down existing threads
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        shuttingDown = true;
    }
    taskAvailable.notify_all();

    for (auto &thread : workers)
    {
        if (thread.joinable())
            thread.join();
    }

    workers.clear();

    {
        std::lock_guard<std::mutex> lock(taskMutex);
        shuttingDown = false;

        // Clear remaining tasks
        std::queue<std::function<void()>> empty;
        std::swap(tasks, empty);

        activeTasks = 0;
    }

    // Start new threads
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back(&DSPThreadPool::workerThread, this);
    }
}

void DSPThreadPool::execute(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        tasks.push(std::move(task));
        activeTasks++;
    }

    taskAvailable.notify_one();
}

void DSPThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(waitMutex);
    allTasksDone.wait(lock, [this]()
                      { return activeTasks.load() == 0; });
}

void DSPThreadPool::workerThread()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(taskMutex);
            taskAvailable.wait(lock, [this]()
                               { return !tasks.empty() || shuttingDown; });

            if (shuttingDown && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        // Run task
        task();

        // Signal task completion
        if (--activeTasks == 0)
        {
            std::lock_guard<std::mutex> lock(waitMutex);
            allTasksDone.notify_all();
        }
    }
}
