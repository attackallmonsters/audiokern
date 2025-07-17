#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>

// Lightweight thread pool for running function pointers with void* context.

class DSPThreadPool
{
public:
    // Type definition for a function that takes a void* context
    using TaskFunc = void (*)(void *);

    // A single task: pointer to object and function to call
    struct Task
    {
        void *object;  // Object to be passed as context
        TaskFunc func; // Function to call with object
    };

    // Constructor: automatically spawns worker threads based on CPU
    DSPThreadPool();

    // Destructor: stops workers and joins threads
    ~DSPThreadPool();

    // Initializes the instance
    void initialize();

    // Add a task (object + function) to the list of persistent jobs
    void setExecute(void *object, TaskFunc func);

    // Push all stored jobs to the queue for execution
    void execute();

    // Wait until all queued jobs have been picked up by worker threads
    void wait();

private:
    std::vector<std::thread> workers;  // Worker threads
    std::vector<Task> jobs;            // Persistent jobs (one-time setup)
    std::queue<Task> queue;            // Work queue for the current block
    std::mutex queueMutex;             // Protects access to the queue
    std::condition_variable condition; // Notifies workers of new tasks
    std::atomic<bool> stop;            // Set to true during shutdown
};
