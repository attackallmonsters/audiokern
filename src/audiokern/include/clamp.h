#pragma once

#include "dsp_types.h"

/**
 * @brief Clamps a value between a minimum and maximum bound.
 *
 * This generic, constexpr-enabled clamp function restricts the given value `val`
 * to the closed interval [`minVal`, `maxVal`].
 *
 * @tparam T Return type (typically the type of `val`)
 * @tparam U Type of the lower bound
 * @tparam V Type of the upper bound
 * @param val The value to clamp
 * @param minVal Minimum allowed value
 * @param maxVal Maximum allowed value
 * @return `val` if within bounds, otherwise the nearest boundary
 */
template <typename T, typename U, typename V>
inline constexpr T clamp(const T &val, const U &minVal, const V &maxVal)
{
    return (val < minVal) ? static_cast<T>(minVal)
                          : ((val > maxVal) ? static_cast<T>(maxVal) : val);
}

/**
 * @brief Ensures a value is not below a minimum bound.
 *
 * This function clamps `val` to be at least `minVal`. Unlike `clamp()`,
 * it does not restrict the upper bound.
 *
 * @tparam T Return type (typically the type of `val`)
 * @tparam U Type of the lower bound
 * @param val The value to clamp
 * @param minVal Minimum allowed value
 * @return `val` if >= `minVal`, otherwise `minVal`
 */
template <typename T, typename U>
inline constexpr T clampmin(const T &val, const U &minVal)
{
    return (val < minVal) ? static_cast<T>(minVal) : val;
}
