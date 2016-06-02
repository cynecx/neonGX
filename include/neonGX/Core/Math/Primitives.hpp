/*
 * neonGX - Primitives.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_PRIMITIVES_H
#define NEONGX_PRIMITIVES_H

#include <cassert>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>
#include <neonGX/Core/Math/Helpers.hpp>

namespace neonGX {

  template <typename T>
  struct PointImpl {
    T x = 0;
    T y = 0;

    PointImpl() = default;

    PointImpl(T x_, T y_) : x{x_}, y{y_} {
    }

    template <typename R>
    PointImpl(const PointImpl<R>& From) : x{T(From.x)}, y{T(From.y)} {
    }

    PointImpl(const PointImpl&) = default;
    PointImpl& operator=(const PointImpl&) = default;

    bool operator==(const PointImpl& rhs) const {
      return std::tie(x, y) == std::tie(rhs.x, rhs.y);
    }

    bool operator!=(const PointImpl& rhs) const {
      return !(*this == rhs);
    }

    bool operator<(const PointImpl& rhs) const {
      return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    bool operator<=(const PointImpl& rhs) const {
      return std::tie(x, y) <= std::tie(rhs.x, rhs.y);
    }

    bool operator>(const PointImpl& rhs) const {
      return std::tie(x, y) > std::tie(rhs.x, rhs.y);
    }

    bool operator>=(const PointImpl& rhs) const {
      return std::tie(x, y) >= std::tie(rhs.x, rhs.y);
    }

    PointImpl& operator+=(const PointImpl& rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
    }

    PointImpl operator+(const PointImpl& rhs) {
      return { x + rhs.x, y + rhs.y };
    }

    PointImpl& operator-=(const PointImpl& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
    }

    PointImpl operator-(const PointImpl& rhs) {
      return { x - rhs.x, y - rhs.y };
    }

    PointImpl& operator*=(const PointImpl& rhs) {
      x *= rhs.x;
      y *= rhs.y;
      return *this;
    }

    PointImpl operator*(const PointImpl& rhs) {
      return { x * rhs.x, y * rhs.y };
    }

    void SetZero() {
      x = y = 0;
    }

    bool IsZero() const {
      return x == 0 && y == 0;
    }

    PointImpl Neg() const {
      return {-x, -y};
    }
  };

  using NPoint = PointImpl<int32_t>;
  using FPoint = PointImpl<float>;

  template <typename T>
  struct SizeImpl {
    T width = 0;
    T height = 0;

    SizeImpl() = default;

    SizeImpl(T width_, T height_) : width{width_}, height{height_} {
    }

    SizeImpl(const SizeImpl&) = default;
    SizeImpl& operator=(const SizeImpl&) = default;

    bool operator==(const SizeImpl& rhs) const {
      return std::tie(width, height) == std::tie(rhs.width, rhs.height);
    }

    bool operator!=(const SizeImpl& rhs) const {
      return !(*this == rhs);
    }

    bool operator<(const SizeImpl& rhs) const {
      return std::tie(width, height) < std::tie(rhs.width, rhs.height);
    }

    bool operator<=(const SizeImpl& rhs) const {
      return std::tie(width, height) <= std::tie(rhs.width, rhs.height);
    }

    bool operator>(const SizeImpl& rhs) const {
      return std::tie(width, height) > std::tie(rhs.width, rhs.height);
    }

    bool operator>=(const SizeImpl& rhs) const {
      return std::tie(width, height) >= std::tie(rhs.width, rhs.height);
    }

    SizeImpl& operator+=(const SizeImpl& rhs) {
      width += rhs.width;
      height += rhs.height;
      return *this;
    }

    SizeImpl operator+(const SizeImpl& rhs) {
      return { width + rhs.width, height + rhs.height };
    }

    SizeImpl& operator-=(const SizeImpl& rhs) {
      width -= rhs.width;
      height -= rhs.height;
      return *this;
    }

    SizeImpl operator-(const SizeImpl& rhs) {
      return { width - rhs.width, height - rhs.height };
    }

    SizeImpl& operator*=(const SizeImpl& rhs) {
      width *= rhs.width;
      height *= rhs.height;
      return *this;
    }

    SizeImpl operator*(const SizeImpl& rhs) {
      return { width * rhs.width, height * rhs.height };
    }

    void SetZero() {
      width = height = T(0);
    }

    bool IsZero() const {
      return width == T(0) && height == T(0);
    }

    bool IsIntegral() const {
      return IsIntegralFP(width) && IsIntegralFP(height);
    }

    template <typename N>
    SizeImpl<N> To() const {
      return SizeImpl<N>(N(width), N(height));
    }
  };

  using NSize = SizeImpl<int32_t>;
  using FSize = SizeImpl<float>;

  struct Vector2 {
    float x = 0;
    float y = 0;

    Vector2() = default;

    Vector2(float x_, float y_) : x{x_}, y{y_} {
    }

    Vector2(const Vector2&) = default;
    Vector2& operator=(const Vector2&) = default;

    bool operator==(const Vector2& rhs) const {
      return std::tie(x, y) == std::tie(rhs.x, rhs.y);
    }

    bool operator!=(const Vector2& rhs) const {
      return !(*this == rhs);
    }

    bool operator<(const Vector2& rhs) const {
      return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    bool operator<=(const Vector2& rhs) const {
      return std::tie(x, y) <= std::tie(rhs.x, rhs.y);
    }

    bool operator>(const Vector2& rhs) const {
      return std::tie(x, y) > std::tie(rhs.x, rhs.y);
    }

    bool operator>=(const Vector2& rhs) const {
      return std::tie(x, y) >= std::tie(rhs.x, rhs.y);
    }

    Vector2& operator+=(const Vector2& rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
    }

    Vector2 operator+(const Vector2& rhs) {
      return { x + rhs.x, y + rhs.y };
    }

    Vector2& operator-=(const Vector2& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
    }

    Vector2 operator-(const Vector2& rhs) {
      return { x - rhs.x, y - rhs.y };
    }

    Vector2& operator*=(const Vector2& rhs) {
      x *= rhs.x;
      y *= rhs.y;
      return *this;
    }

    Vector2 operator*(const Vector2& rhs) {
      return { x * rhs.x, y * rhs.y };
    }

    void SetZero() {
      x = y = 0;
    }

    bool IsZero() const {
      return x == 0.0f && y == 0.0f;
    }
  };

  struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;

    Vector3(float x_, float y_, float z_) : x{x_}, y{y_}, z{z_} {
    }

    Vector3(const Vector3&) = default;
    Vector3& operator=(const Vector3&) = default;

    bool operator==(const Vector3& rhs) const {
      return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
    }

    bool operator!=(const Vector3& rhs) const {
      return !(*this == rhs);
    }

    bool operator<(const Vector3& rhs) const {
      return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
    }

    bool operator<=(const Vector3& rhs) const {
      return std::tie(x, y, z) <= std::tie(rhs.x, rhs.y, rhs.z);
    }

    bool operator>(const Vector3& rhs) const {
      return std::tie(x, y, z) > std::tie(rhs.x, rhs.y, rhs.z);
    }

    bool operator>=(const Vector3& rhs) const {
      return std::tie(x, y, z) >= std::tie(rhs.x, rhs.y, rhs.z);
    }

    Vector3& operator+=(const Vector3& rhs) {
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;
      return *this;
    }

    Vector3 operator+(const Vector3& rhs) {
      return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    Vector3& operator-=(const Vector3& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      z -= rhs.z;
      return *this;
    }

    Vector3 operator-(const Vector3& rhs) {
      return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    Vector3& operator*=(const Vector3& rhs) {
      x *= rhs.x;
      y *= rhs.y;
      z *= rhs.z;
      return *this;
    }

    Vector3 operator*(const Vector3& rhs) {
      return { x * rhs.x, y * rhs.y, z * rhs.z };
    }

    void SetZero() {
      x = y = z = 0;
    }

    bool IsZero() const {
      return x == 0.0f && y == 0.0f && z == 0.0f;
    }
  };

  template <typename PointTy, typename SizeTy>
  struct RectangleImpl {
    using Point = PointImpl<PointTy>;
    using Size = SizeImpl<SizeTy>;

    Point point{};
    Size size{};

    RectangleImpl() = default;

    RectangleImpl(const Point& p, const Size& s) : point(p), size(s) {
    }

    RectangleImpl(const RectangleImpl&) = default;
    RectangleImpl& operator=(const RectangleImpl&) = default;

    bool operator==(const RectangleImpl& rhs) const {
      return point == rhs.point && size == rhs.size;
    }

    bool operator!=(const RectangleImpl& rhs) const {
      return !(*this == rhs);
    }

    bool Intersect(const RectangleImpl& rec) {
      auto valIsInRange = [] (int value, int min, int max) -> bool {
        return (value >= min) && (value <= max);
      };

      bool xOverlap =
          valIsInRange(point.x, rec.point.x,rec.point.x + rec.size.width) ||
              valIsInRange(rec.point.x, point.x, point.x + size.width);

      bool yOverlap =
          valIsInRange(point.y, rec.point.y,rec.point.y + rec.size.height) ||
              valIsInRange(rec.point.y, point.y, point.y + size.height);

      return xOverlap && yOverlap;
    }

    bool Contains(const Point& p) const {
      if(size <= Size{0, 0}) {
        return false;
      }

      return p.x >= point.x && p.x < point.x + size.width
             && p.y >= point.y && p.y < point.y + size.height;
    }

    bool IsNull() const {
      return point == Point{PointTy(0), PointTy(0)} &&
          size == Size{SizeTy(0), SizeTy(0)};
    }
  };

  using NRectangle = RectangleImpl<int32_t, int32_t>;
  using FRectangle = RectangleImpl<float, float>;

} // end namespace neonGX

#endif // !NEONGX_PRIMITIVES_H
