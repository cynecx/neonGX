/*
 * neonGX - PNGImage.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_PNGIMAGE_H
#define NEONGX_PNGIMAGE_H

#include <neonGX/Core/Math/Helpers.hpp>
#define PNG_READ_16_TO_8_SUPPORTED
#ifdef NEONGX_USE_EMSCRIPTEN
#include <pngpp/png.hpp>
#else
#include <png++/png.hpp>
#endif
#include <fstream>
#include <vector>
#include <memory>

namespace neonGX {

  struct PNGImage {
    size_t Width = 0;
    size_t Height = 0;

    png::color_type ColorType = png::color_type::color_type_none;
    const png::color_type FormatType = png::color_type::color_type_rgba;

    std::vector<uint8_t> RawData;

    void Reset() {
      Width = 0;
      Height = 0;

      ColorType = png::color_type::color_type_none;
      RawData.clear();
    }

    bool SizeIsPowerOfTwo() const {
      return IsPowerOfTwo(Width) && IsPowerOfTwo(Height);
    }
  };

  bool LoadPNGImage(std::ifstream& stream, PNGImage& output);
  bool LoadPNGImage(std::string filename, PNGImage& output);

  std::shared_ptr<PNGImage> LoadPNGImageShared(std::string filename);
  std::shared_ptr<PNGImage> LoadPNGImageShared(std::ifstream& stream);

} // end namespace neonGX

#endif // !NEONGX_PNGIMAGE_H
