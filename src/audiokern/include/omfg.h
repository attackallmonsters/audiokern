#pragma once

#include <sstream>
#include <stdexcept>

/**
 * @brief A humorous wrapper around std::ostringstream for throwing runtime errors.
 *
 * Build your fatal error message using stream syntax, then call panic() to throw it.
 * Example:
 *     omfgstream() << "Missing output bus in module " << getName() << ".";
 */
class omfg : public std::ostringstream
{
};

/**
 * @brief Throws the built message as a std::runtime_error.
 *
 * Must be called at the end of the stream chain.
 * Marked [[noreturn]] to help static analysis.
 */
[[noreturn]] inline void panic(const std::ostringstream &oss)
{
    throw std::runtime_error(oss.str());
}