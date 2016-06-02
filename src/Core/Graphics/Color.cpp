/*
 * neonGX - Color.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Graphics/Color.hpp>
#include <boost/format.hpp>

namespace neonGX {

  std::string ColorRGB::ToString() const {
    using namespace std;
    using namespace boost;

    stringstream outputStr;
    outputStr << format("ColorRGB{r: %1%, g: %2%, b: %3%}")
                 % int(r) % int(g) % int(b);

    return outputStr.str();
  }

  std::string ColorRGBA::ToString() const {
    using namespace std;
    using namespace boost;

    stringstream outputStr;
    outputStr << format("ColorRGBA{r: %1%, g: %2%, b: %3%, a: %4%}")
                 % int(r * 255.0f) % int(g * 255.0f)
                 % int(b * 255.0f) % a;

    return outputStr.str();
  }
}
