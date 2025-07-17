#include "DSPThreadPool.h"

DSPThreadPool::DSPThreadPool()
{
    unsigned int maxThreads = std::max(1u, std::thread::hardware_concurrency() / 2);
    stop = false;

    for (unsigned int i = 0; i < maxThreads; ++i)
    {
        workers.emplace_back([this]()
                             {
                while (true) {
                    Task task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() {
                            return stop || !queue.empty();
                        });

                        if (stop && queue.empty())
                            return;

                        task = queue.front();
                        queue.pop();
                    }

                    task.func(task.object);
                } });
    }
}

DSPThreadPool::~DSPThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();
    for (auto &worker : workers)
        if (worker.joinable())
            worker.join();
}

void DSPThreadPool::initialize()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        jobs.clear(); // Remove all configured persistent jobs
        std::queue<Task> empty;
        std::swap(queue, empty); // Clear the queue efficiently
    }
}

void DSPThreadPool::setExecute(void *object, TaskFunc func)
{
    jobs.push_back({object, func});
}

void DSPThreadPool::execute()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        for (const auto &task : jobs)
            queue.push(task);
    }
    condition.notify_all();
}

void DSPThreadPool::wait()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (queue.empty())
                break;
        }
        std::this_thread::yield();
    }
}