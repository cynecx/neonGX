/*
 * neonGX - Helpers.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_HELPERS_H
#define NEONGX_HELPERS_H

#include <cassert>
#include <cstdint>
#include <climits>
#include <cmath>
#include <type_traits>

namespace neonGX {

  // Source: https://graphics.stanford.edu/~seander/bithacks.html

  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr bool Is16BitInteger = (sizeof(T) == 2);

  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr bool Is32BitInteger = (sizeof(T) == 4);

  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr bool Is64BitInteger = (sizeof(T) == 8);

  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  inline constexpr bool IsPowerOfTwo(T val) {
    return (val & (val - 1)) == 0;
  }

  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  inline constexpr T NextPowerOf2(T val) {
    --val;
    for (uint8_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2) {
      val |= val >> i;
    }
    return ++val;
  }

  template <typename T>
  inline constexpr T ExtractSign(T val) {
    if(val == 0) {
      return 0;
    }

    return (val < 0) ? T(-1) : T(1);
  }

  template <typename R, typename T,
      typename = std::enable_if_t<std::is_floating_point<T>::value>>
  inline constexpr R GetIntegralFromFP(T val) {
    T integral = 0;
    auto result = std::modf(val, &integral);
    assert(result == 0.0);
    return R(integral);
  }

  template <typename T,
      typename = std::enable_if_t<std::is_floating_point<T>::value>>
  inline constexpr bool IsIntegralFP(T val) {
    T integral = 0;
    return std::modf(val, &integral) == 0.0;
  }


} // end namespace neonGX

#endif // !NEONGX_HELPERS_H
