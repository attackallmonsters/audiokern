#pragma once

inline unsigned int cpu_count()
{
    return std::thread::hardware_concurrency();
}