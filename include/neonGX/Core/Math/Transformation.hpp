/*
 * neonGX - Transformations.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TRANSFORMATIONS_H
#define NEONGX_TRANSFORMATIONS_H

#include <cmath>
#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Math/Matrix.hpp>
#include <boost/math/constants/constants.hpp>

//#define USE_CONSTANT_CACHING

namespace neonGX {

  enum class ConstantCacheType {
    sin,
    cos,
    tan
  };

  float ConstantCache(ConstantCacheType type, float val);

  template <typename T>
  static constexpr T PI = boost::math::constants::pi<T>();

  inline constexpr float DegreeToRadian(float degree) {
    return degree * PI<float> / 180.0f;
  }

  inline constexpr float RadianToDegree(float radian) {
    return radian * 180.0f / PI<float>;
  }

  inline constexpr Matrix3 GetTranslationMatrix(float tx, float ty) {
    return {
        1.0f, 0.0f, tx,
        0.0f, 1.0f, ty,
        0.0f, 0.0f, 1.0f
    };
  }

  inline constexpr Matrix3 GetTranslationMatrix(const FPoint& point) {
    return GetTranslationMatrix(point.x, point.y);
  }

  inline constexpr Matrix3 GetScalingMatrix(float sx, float sy) {
    return {
        sx, 0.0f, 0.0f,
        0.0f, sy, 0.0f,
        0.0f, 0.0f, 1.0f
    };
  }

  inline constexpr Matrix3 GetScalingMatrix(const FPoint& point) {
    return GetScalingMatrix(point.x, point.y);
  }

  inline Matrix3 GetRotationMatrix(float q) {
#ifdef USE_CONSTANT_CACHING
    float qsin = ConstantCache(ConstantCacheType::sin, q);
    float qcos = ConstantCache(ConstantCacheType::cos, q);
#else
    float qsin = sin(q);
    float qcos = cos(q);
#endif

    return {
        qcos, -qsin, 0.0f,
        qsin, qcos, 0.0f,
        0.0f, 0.0f, 1.0f
    };
  }

  inline Matrix3 GetSkewingMatrix(float x, float y) {
#ifdef USE_CONSTANT_CACHING
    float xtan = ConstantCache(ConstantCacheType::tan, x);
    float ytan = ConstantCache(ConstantCacheType::tan, y);
#else
    float xtan = tan(x);
    float ytan = tan(y);
#endif

    return {
        1.0f, xtan, 0.0f,
        ytan, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
  }

  inline Matrix3 GetSkewingMatrix(const FPoint& point) {
    return GetSkewingMatrix(point.x, point.y);
  }

  inline constexpr Matrix3 GetIdentityTransform() {
    return {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
  }

  inline Matrix3 GetToClipspaceTransform(const FSize& destFrame,
                                         const FPoint& sourcePoint,
                                         bool swapHeightSign) {
    // (Position / Resolution) * 2 - 1
    auto toClipspace = Matrix3{
        2.0f / destFrame.width, 0.0f, -1.0f,
        0.0f, (swapHeightSign ? -2.0f : 2.0f) / destFrame.height,
          (swapHeightSign ? 1.0f : -1.0f),
        0.0f, 0.0f, 1.0f
    };

    return toClipspace * GetTranslationMatrix(-sourcePoint.x, -sourcePoint.y);
  }

} // end namespace neonGX

#endif // !NEONGX_TRANSFORMATIONS_H
