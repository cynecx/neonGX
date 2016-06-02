/*
 * neonGX - Timer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TIMER_H
#define NEONGX_TIMER_H

#include <cstdint>
#include <chrono>

namespace neonGX {

  class Timer {
  private:
    using ClockTy = std::chrono::high_resolution_clock;
    std::chrono::time_point<ClockTy> m_Begin;

  public:
    Timer() : m_Begin(ClockTy::now()) {
    }

    void Reset() {
      m_Begin = ClockTy::now();
    }

    auto Elapsed() const {
      using namespace std::chrono;
      return duration_cast<milliseconds>(ClockTy::now() - m_Begin).count();
    }

    template <typename T = uint32_t>
    T ElapsedAndReset() {
      using namespace std::chrono;
      std::chrono::duration<T, std::milli> delta = ClockTy::now() - m_Begin;
      m_Begin = ClockTy::now();
      return delta.count();
    }
  };


} // end namespace neonGX

#endif // !NEONGX_TIMER_H
