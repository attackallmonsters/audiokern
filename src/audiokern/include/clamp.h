#pragma once

#include "dsp_types.h"
template <typename T, typename U, typename V>
// Clamp min/max function
inline constexpr T clamp(const T &val, const U &minVal, const V &maxVal)
{

    return (val < minVal) ? static_cast<T>(minVal) : ((val > maxVal) ? static_cast<T>(maxVal) : val);
}

// Clamp min function
template <typename T, typename U>
inline constexpr T clampmin(const T &val, const U &minVal)
{
    return (val < minVal) ? static_cast<T>(minVal) : val;
}