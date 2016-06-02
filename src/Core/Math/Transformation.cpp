/*
 * neonGX - Transformations.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Math/Transformation.hpp>
#include <cmath>
#include <unordered_map>

namespace neonGX {

  namespace {
    std::unordered_map<float, float> sinCache{};
    std::unordered_map<float, float> cosCache{};
    std::unordered_map<float, float> tanCache{};
  }

  float ConstantCache(ConstantCacheType type, float val) {
    float retVal = 0;

    switch(type) {
      case ConstantCacheType::sin: {
        auto it = sinCache.find(val);
        if(it == sinCache.end()) {
          retVal = std::sin(val);
          sinCache.emplace(val, retVal);
        } else {
          retVal = it->second;
        }
        break;
      }
      case ConstantCacheType::cos: {
        auto it = cosCache.find(val);
        if(it == cosCache.end()) {
          retVal = std::sin(val);
          cosCache.emplace(val, retVal);
        } else {
          retVal = it->second;
        }
        break;
      }
      case ConstantCacheType::tan: {
        auto it = tanCache.find(val);
        if(it == tanCache.end()) {
          retVal = std::sin(val);
          tanCache.emplace(val, retVal);
        } else {
          retVal = it->second;
        }
        break;
      }
    }

    return retVal;
  }

}
