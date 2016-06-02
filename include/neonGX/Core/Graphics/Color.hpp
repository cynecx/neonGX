/*
 * neonGX - Color.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_COLOR_H
#define NEONGX_COLOR_H

#include <cstdint>
#include <cmath>
#include <string>
#include <sstream>
#include <neonGX/Core/ADT.hpp>

namespace neonGX {

  struct ColorRGB {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    ColorRGB() = default;

    ColorRGB(uint8_t red, uint8_t green, uint8_t blue) :
        r{red}, g{green}, b{blue} {
    }

    ColorRGB(uint8_t red, uint8_t green, uint8_t blue, float alpha)
        : ColorRGB(red, green, blue) {
      AdjustAlpha(alpha);
    }

    ColorRGB(const ColorRGB&) = default;
    ColorRGB& operator=(const ColorRGB&) = default;

    void AdjustAlpha(float alpha) {
      r = uint8_t(std::round(alpha * r));
      g = uint8_t(std::round(alpha * g));
      b = uint8_t(std::round(alpha * b));
    }

    static optional<ColorRGB> FromHex(const std::string& str) {
      if(str.size() != 7 || str[0] != '#') {
        return nullopt;
      }

      ColorRGB returnVal;
      returnVal.r = uint8_t(std::stoi(str.substr(1, 2), nullptr, 16));
      returnVal.g = uint8_t(std::stoi(str.substr(3, 2), nullptr, 16));
      returnVal.b = uint8_t(std::stoi(str.substr(5, 2), nullptr, 16));

      return returnVal;
    }

    std::string ToString() const;
  };

  struct ColorRGBA {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1;

    ColorRGBA() = default;

    ColorRGBA(float red, float green, float blue, float alpha) :
        r{red}, g{green}, b{blue}, a{alpha} {
    }

    ColorRGBA(const ColorRGBA&) = default;
    ColorRGBA& operator=(const ColorRGBA&) = default;

    ColorRGBA(const ColorRGB& color)
        : r{float(color.r) / 255.0f},
          g{float(color.g) / 255.0f},
          b{float(color.b) / 255.0f} {
    }

    static optional<ColorRGBA> FromHex(const std::string& str) {
      if(str.size() != 7 || str[0] != '#') {
        return nullopt;
      }

      ColorRGBA returnVal;
      returnVal.r = float(std::stoi(str.substr(1, 2), nullptr, 16)) / 255.0f;
      returnVal.g = float(std::stoi(str.substr(3, 2), nullptr, 16)) / 255.0f;
      returnVal.b = float(std::stoi(str.substr(5, 2), nullptr, 16)) / 255.0f;

      return returnVal;
    }

    std::string ToString() const;
  };

} // end namespace neonGX

#endif // !NEONGX_COLOR_H
