#pragma once

#include <sstream>
#include <stdexcept>

/**
 * @brief Panic macro for convenient runtime error reporting using stream syntax.
 *
 * Usage:
 *     PANIC("Missing bus: " << name << ", module: " << moduleName);
 *
 * Internally uses std::ostringstream and throws std::runtime_error.
 */
#define PANIC(msg)                                                   \
    do {                                                             \
        std::ostringstream _panic_stream_;                           \
        _panic_stream_ << msg;                                       \
        throw std::runtime_error(_panic_stream_.str());              \
    } while (0)
