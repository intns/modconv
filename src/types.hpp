#pragma once

#include <cstdbool>
#include <cstdint>
#include <cmath>
#include <limits>
#include <type_traits>

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using vu8  = volatile u8;
using vu16 = volatile u16;
using vu32 = volatile u32;
using vu64 = volatile u64;
using vs8  = volatile s8;
using vs16 = volatile s16;
using vs32 = volatile s32;
using vs64 = volatile s64;

using f32  = float;
using f64  = double;
using vf32 = volatile f32;
using vf64 = volatile f64;

/**
 * @brief Compares two floating-point numbers for near equality.
 * @tparam T A floating-point type (float or double).
 * @param a The first value.
 * @param b The second value.
 * @param epsilon The tolerance for the comparison.
 * @return True if the values are within the epsilon tolerance of each other.
 */
template <typename T>
inline bool nearly_equal(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
{
	static_assert(std::is_floating_point_v<T>, "nearly_equal should only be used with floating-point types.");
	return std::abs(a - b) <= epsilon;
}

#ifdef __cplusplus
static_assert(sizeof(s8) == 1 && sizeof(u8) == 1, "8 bit number isn't 1 byte big");
static_assert(sizeof(s16) == 2 && sizeof(u16) == 2, "16 bit number isn't 2 bytes big");
static_assert(sizeof(s32) == 4 && sizeof(u32) == 4, "32 bit number isn't 4 bytes big");
static_assert(sizeof(s64) == 8 && sizeof(u64) == 8, "64 bit number isn't 8 bytes big");
static_assert(sizeof(f32) == 4, "32 bit floating point number isn't 4 bytes big");
static_assert(sizeof(f64) == 8, "64 bit floating point number isn't 8 bytes big");
#endif