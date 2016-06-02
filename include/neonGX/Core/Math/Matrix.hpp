/*
 * neonGX - Matrix.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_MATRIX_H
#define NEONGX_MATRIX_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <type_traits>

#include <neonGX/Core/Math/Primitives.hpp>

namespace neonGX {

  template <uint16_t M, uint16_t N = M, typename T = float>
  struct Matrix {
    using index_t = uint16_t;

    static_assert(M >= 1 && N >= 1);
    static constexpr size_t ElementsCount = M * N;

    using matrix_ty = std::array<T, ElementsCount>;
    using iterator = typename matrix_ty::iterator;
    using const_iterator = typename matrix_ty::const_iterator;

    template <typename Accessor, index_t Limit>
    struct MatrixAccessorIterator {
    private:
      const Accessor m_Accessor;
      index_t m_CurrIndex;

    public:
      constexpr MatrixAccessorIterator(const Accessor& accessor,
                             index_t currIndex)
          : m_Accessor{accessor}, m_CurrIndex{currIndex} {
        assert(currIndex <= Limit);
      }

      constexpr MatrixAccessorIterator(const MatrixAccessorIterator&) = default;

      MatrixAccessorIterator& operator++() {
        ++m_CurrIndex;
        assert(m_CurrIndex <= Limit);
        return *this;
      }

      MatrixAccessorIterator operator++(int) {
        MatrixAccessorIterator tmp(*this);
        ++m_CurrIndex;
        assert(m_CurrIndex <= Limit);
        return tmp;
      }

      bool operator==(const MatrixAccessorIterator& rhs) const {
        return m_Accessor == rhs.m_Accessor && m_CurrIndex == rhs.m_CurrIndex;
      }

      bool operator!=(const MatrixAccessorIterator& rhs) const {
        return !(*this == rhs);
      }

      T operator*() const {
        return m_Accessor[m_CurrIndex];
      }

      T operator->() const {
        return m_Accessor[m_CurrIndex];
      }
    };

    template <typename MatTy>
    struct MatrixRowAccessor {
      using IteratorTy = MatrixAccessorIterator<MatrixRowAccessor, N>;
      using ValTy = std::conditional_t<std::is_const<MatTy>::value,
          std::add_const_t<T>, T>;

    private:
      MatTy& m_ArrRef;
      const size_t m_RowIndex;

    public:
      constexpr MatrixRowAccessor(MatTy& arr, index_t row)
          : m_ArrRef{arr}, m_RowIndex(row * N) {
        assert(row < M);
      }

      constexpr MatrixRowAccessor(const MatrixRowAccessor&) = default;

      constexpr size_t Size() const {
        return N;
      }

      const T& operator[](index_t column) const {
        assert(column < N);
        return m_ArrRef[m_RowIndex + column];
      }

      ValTy& operator[](index_t column) {
        assert(column < N);
        return m_ArrRef[m_RowIndex + column];
      }

      bool operator==(const MatrixRowAccessor& rhs) const {
        return &m_ArrRef == &rhs.m_ArrRef && m_RowIndex == rhs.m_RowIndex;
      }

      bool operator!=(const MatrixRowAccessor& rhs) const {
        return !(*this == rhs);
      }

      IteratorTy begin() const {
        return {*this, 0};
      }

      IteratorTy end() const {
        return {*this, N};
      }
    };

    template <typename MatTy>
    struct MatrixColumnAccessor {
      using IteratorTy = MatrixAccessorIterator<MatrixColumnAccessor, M>;

    private:
      MatTy& m_ArrRef;
      const index_t m_Column;

    public:
      constexpr MatrixColumnAccessor(MatTy& arr, index_t column)
          : m_ArrRef{arr}, m_Column{column} {
        assert(column < N);
      }

      constexpr MatrixColumnAccessor(const MatrixColumnAccessor& rhs) = default;

      constexpr size_t Size() const {
        return M;
      }

      const T& operator[](index_t row) const {
        assert(row < M);
        return m_ArrRef[row * N + m_Column];
      }

      T& operator[](index_t row) {
        assert(row < M);
        return m_ArrRef[row * N + m_Column];
      }

      bool operator==(const MatrixColumnAccessor& rhs) const {
        return &m_ArrRef == &rhs.m_ArrRef && m_Column == rhs.m_Column;
      }

      bool operator!=(const MatrixColumnAccessor& rhs) const {
        return !(*this == rhs);
      }

      IteratorTy begin() const {
        return {*this, 0};
      }

      IteratorTy end() const {
        return {*this, M};
      }
    };

    matrix_ty m_Values = { {0} };

    constexpr Matrix() = default;

    template <typename... ElemsTy>
    constexpr Matrix(ElemsTy... values) : m_Values{{T{values}...}} {
      static_assert(sizeof...(ElemsTy) == ElementsCount);
    }

    constexpr Matrix(const Matrix&) = default;
    constexpr Matrix& operator=(const Matrix&) = default;

    const MatrixRowAccessor<const matrix_ty> GetRowAccessor(
        index_t row) const {
      return {m_Values, row};
    }

    MatrixRowAccessor<matrix_ty> GetRowAccessor(index_t row) {
      return {m_Values, row};
    }

    const MatrixColumnAccessor<const matrix_ty> GetColumnAccessor(
        index_t column) const {
      return {m_Values, column};
    }

    MatrixColumnAccessor<matrix_ty> GetColumnAccessor(
        index_t column) {
      return {m_Values, column};
    }

    const MatrixRowAccessor<const matrix_ty> operator[](
        index_t row) const {
      return GetRowAccessor(row);
    }

    MatrixRowAccessor<matrix_ty> operator[](index_t row) {
      return GetRowAccessor(row);
    }

    iterator begin() {
      return m_Values.begin();
    }

    const_iterator begin() const {
      return m_Values.begin();
    }

    iterator end() {
      return m_Values.end();
    }

    const_iterator end() const {
      return m_Values.end();
    }

    Matrix& operator*=(const T& rhs) {
      std::transform(begin(), end(), begin(), [&rhs] (T val) {
        return val * rhs;
      });

      return *this;
    }

    Matrix operator*(T rhs) {
      Matrix result;

      std::transform(begin(), end(), result.begin(), [&rhs] (T val) {
        return val * rhs;
      });

      return result;
    }

    Matrix& operator/=(const T& rhs) {
      std::transform(begin(), end(), begin(), [&rhs] (T val) {
        return val / rhs;
      });

      return *this;
    }

    Matrix operator/(T rhs) {
      Matrix result;

      std::transform(begin(), end(), result.begin(), [&rhs] (T val) {
        return val / rhs;
      });

      return result;
    }

    Matrix& operator+=(const Matrix& rhs) {
      std::transform(begin(), end(), rhs.begin(), begin(), [] (T val, T val2) {
        return val + val2;
      });

      return *this;
    }

    Matrix operator+(const Matrix& rhs) {
      Matrix result;

      std::transform(begin(), end(), rhs.begin(), result.begin(),
          [] (T val, T val2) {
            return val + val2;
          }
      );

      return result;
    }

    Matrix& operator-=(const Matrix& rhs) {
      std::transform(begin(), end(), rhs.begin(), begin(), [] (T val, T val2) {
        return val - val2;
      });

      return *this;
    }

    Matrix operator-(const Matrix& rhs) {
      Matrix result;

      std::transform(begin(), end(), rhs.begin(), result.begin(),
          [] (T val, T val2) {
            return val - val2;
          }
      );

      return result;
    }

    template <uint16_t N2>
    Matrix<M, N2, T> operator*(const Matrix<N, N2, T>& rhs) {
      Matrix<M, N2, T> result;

      for(index_t y = 0; y < M; y++) {
        auto rowAccessor = GetRowAccessor(y);
        auto outRowAccessor = result.GetRowAccessor(y);
        for(index_t x = 0; x < N2; x++) {
          outRowAccessor[x] = std::inner_product(rowAccessor.begin(),
              rowAccessor.end(), rhs.GetColumnAccessor(x).begin(), T(0));
        }
      }

      return result;
    }

    template<typename _InputIterator1, typename _InputIterator2, typename _Tp>
    __attribute__((always_inline)) _Tp
    inner_product2(_InputIterator1 __first1, _InputIterator1 __last1,
                  _InputIterator2 __first2, _Tp __init)
    {
      for (; __first1 != __last1; ++__first1, (void)++__first2)
        __init = __init + (*__first1 * *__first2);
      return __init;
    }

    Matrix& operator*=(const Matrix& rhs) {
      Matrix result;

      for(index_t y = 0; y < M; y++) {
        auto rowAccessor = GetRowAccessor(y);
        auto outRowAccessor = result.GetRowAccessor(y);
        for(index_t x = 0; x < N; x++) {
          outRowAccessor[x] = inner_product2(rowAccessor.begin(),
              rowAccessor.end(), rhs.GetColumnAccessor(x).begin(), T(0));
        }
      }

      m_Values = result.m_Values;

      return *this;
    }
  };

  using Matrix2 = Matrix<2>;
  using Matrix3 = Matrix<3>;
  using Matrix4 = Matrix<4>;

  inline void SetIdentityMatrix(Matrix3& mat) {
    auto rowAccess0 = mat.GetRowAccessor(0);
    auto rowAccess1 = mat.GetRowAccessor(1);
    auto rowAccess2 = mat.GetRowAccessor(2);

    rowAccess0[0] = 1.0f;
    rowAccess0[1] = 0.0f;
    rowAccess0[2] = 0.0f;

    rowAccess1[0] = 0.0f;
    rowAccess1[1] = 1.0f;
    rowAccess1[2] = 0.0f;

    rowAccess2[0] = 0.0f;
    rowAccess2[1] = 0.0f;
    rowAccess2[2] = 1.0f;
  }

  inline Matrix<3, 1, float> PointToMatrix3x1(const FPoint& point) {
    return { point.x, point.y, 1.0f };
  }

  inline Matrix3 GetInverseMatrix(const Matrix3& mat) {
    float a = mat[0][0];
    float b = mat[0][1];
    float c = mat[0][2];

    float d = mat[1][0];
    float e = mat[1][1];
    float f = mat[1][2];

    float g = mat[2][0];
    float h = mat[2][1];
    float i = mat[2][2];

    // Sauce: Wolfram Alpha

    return {
        (e*i-f*h) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (c*h-b*i) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (b*f-c*e) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (f*g-d*i) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (a*i-c*g) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (c*d-a*f) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (d*h-e*g) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (b*g-a*h) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
        (a*e-b*d) / (-c*e*g+b*f*g+c*d*h-a*f*h-b*d*i+a*e*i),
    };
  }

  inline FPoint ApplyTransformation(const Matrix3& transform,
                                    const FPoint& point) {
    auto rowAccess0 = transform.GetRowAccessor(0);
    auto rowAccess1 = transform.GetRowAccessor(1);

    return {
        rowAccess0[0] * point.x + rowAccess0[1] * point.y + rowAccess0[2],
        rowAccess1[0] * point.x + rowAccess1[1] * point.y + rowAccess1[2]
    };
  }

  inline FPoint ApplyInverseTransformation(const Matrix3& transform,
                                           const FPoint& point) {
    auto inverseMat = GetInverseMatrix(transform);
    return ApplyTransformation(inverseMat, point);
  }

} // end namespace neonGX

#endif // !NEONGX_MATRIX_H
