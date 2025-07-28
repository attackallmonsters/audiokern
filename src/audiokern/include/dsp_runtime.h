#pragma once

#include <thread>

/**
 * @brief Returns the number of hardware CPU cores available.
 *
 * Uses `std::thread::hardware_concurrency()` to query the number of concurrent
 * threads supported by the system (typically equal to the number of logical cores).
 *
 * @return Number of hardware threads (may return 0 if unknown).
 */
inline unsigned int cpu_count()
{
    return std::thread::hardware_concurrency();
}
