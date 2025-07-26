#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

/**
 * @brief Lightweight thread pool for parallel task execution in DSP systems.
 *
 * This class manages a fixed number of worker threads and allows scheduling
 * arbitrary tasks (`std::function<void()>`) for background processing.
 *
 * It is specifically designed for audio/DSP contexts, where control over
 * task completion (via `wait()`) is important and deterministic lifecycles
 * are preferred over dynamic spawning.
 *
 * Usage:
 * @code
 * DSPThreadPool pool;
 * pool.initialize(cpu_count());  // Start one thread per logical core
 *
 * for (int i = 0; i < 100; ++i)
 * {
 *     pool.execute([=]() {
 *         do_something_heavy(i);
 *     });
 * }
 *
 * pool.wait(); // Block until all tasks have finished
 * @endcode
 *
 * @note Tasks should be short-lived and non-blocking to avoid deadlocks or starvation.
 * @note This thread pool is not intended for real-time audio threads (e.g. audio callbacks).
 */
class DSPThreadPool
{
public:
    /**
     * @brief Constructs an empty thread pool.
     *
     * Threads are not started until `initialize()` is called.
     */
    DSPThreadPool();

    /**
     * @brief Destructor – stops all threads and cleans up pending tasks.
     */
    ~DSPThreadPool();

    /**
     * @brief Starts the thread pool with a fixed number of worker threads.
     *
     * Can only be called once. Calling this multiple times is undefined.
     *
     * @param numThreads Number of threads to create.
     */
    void initialize(size_t numThreads);

    /**
     * @brief Submits a task to be executed asynchronously by the pool.
     *
     * @param task Callable object with no parameters and no return value.
     *
     * Usage:
     * @code
     * DSPThreadPool pool;
     * pool.initialize(cpu_count());  // Start one thread per logical core
     *
     * for (int i = 0; i < 100; ++i)
     * {
     *     pool.execute([=]() {
     *         do_something_heavy(i);
     *     });
     * }
     */
    void execute(std::function<void()> task);

    /**
     * @brief Blocks until all currently submitted tasks are completed.
     *
     * Useful to synchronize results or ensure all parallel operations are done
     * before continuing (e.g., in block-based DSP workflows).
     */
    void wait();

    // Prevent copy construction and assignment
    DSPThreadPool(const DSPThreadPool &) = delete;
    DSPThreadPool &operator=(const DSPThreadPool &) = delete;

private:
    /**
     * @brief The worker loop executed by each thread.
     *
     * Waits for new tasks, executes them, and signals completion.
     */
    void workerThread();

    std::vector<std::thread> workers;        ///< Vector of worker threads
    std::queue<std::function<void()>> tasks; ///< FIFO queue of pending tasks

    std::mutex taskMutex;                  ///< Mutex for synchronizing task queue
    std::condition_variable taskAvailable; ///< Signaled when a task is available

    std::atomic<int> activeTasks;     ///< Number of tasks currently running
    std::mutex waitMutex;                 ///< Mutex for wait condition
    std::condition_variable allTasksDone; ///< Signaled when all tasks are completed

    bool shuttingDown = false; ///< True if the thread pool is being destroyed
};
