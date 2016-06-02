/*
 * neonGX - PNGImage.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Image/PNGImage.hpp>

namespace neonGX {

  bool LoadPNGImage(std::ifstream& stream, PNGImage& output) {
    using namespace png;

    using pixel_ty = rgba_pixel;
    using traits = pixel_traits<pixel_ty>;

    assert(stream.is_open());

    try {
      reader<std::ifstream> rd(stream);
      rd.read_info();

      const auto& transform = convert_color_space<pixel_ty>();
      transform(rd);

#if __BYTE_ORDER == __LITTLE_ENDIAN
      if (traits::get_bit_depth() == 16) {
        rd.set_swap();
      }
#endif

      size_t pass_count = 1;
      if (rd.get_interlace_type() != interlace_none) {
        pass_count = size_t(rd.set_interlace_handling());
      }

      rd.update_info();

      if (rd.get_color_type() != traits::get_color_type()
          || rd.get_bit_depth() != traits::get_bit_depth()) {
        throw std::logic_error("color type and/or bit depth mismatch in "
            "png::consumer::read()");
      }

      const auto& img_info = rd.get_image_info();

      size_t imgWidth = img_info.get_width();
      size_t imgHeight = img_info.get_height();

      output.ColorType = img_info.get_color_type();
      output.Width = imgWidth;
      output.Height = imgHeight;

      if (pass_count > 1) {
        std::vector<pixel_ty> dummy_row(imgWidth);
        for (size_t pass = 1; pass < pass_count; ++pass) {
          rd.read_row(reinterpret_cast<byte*>(dummy_row.data()));
        }
      }

      png_structp ptrPngStruct = rd.get_png_struct();
      png_infop ptrPngInfo = rd.get_info().get_png_info();

      const png_size_t rowSize = png_get_rowbytes(ptrPngStruct, ptrPngInfo);
      assert(rowSize > 0);

      const size_t rawDataSize = rowSize * imgHeight;
      output.RawData.clear();
      output.RawData.resize(rawDataSize);

      std::vector<png_byte*> rowPointers;
      rowPointers.resize(imgHeight);

      uint8_t* ptrData = output.RawData.data();

      for(size_t i = 0; i < imgHeight; i++) {
        rowPointers[i] = ptrData + i * rowSize;
      }

      png_read_image(rd.get_png_struct(), rowPointers.data());

      rd.read_end_info();
    } catch (...) {
      output.Width = 0;
      output.Height = 0;
      output.ColorType = color_type::color_type_none;
      output.RawData.clear();
      return false;
    }

    return true;
  }

  bool LoadPNGImage(std::string fileName, PNGImage& output) {
    std::ifstream inputStream(fileName);
    assert(inputStream.is_open());
    return LoadPNGImage(inputStream, output);
  }

  std::shared_ptr<PNGImage> LoadPNGImageShared(std::string filename) {
    std::shared_ptr<PNGImage> pngImg = std::make_shared<PNGImage>();
    bool result = LoadPNGImage(filename, *pngImg.get());
    return result ? pngImg : nullptr;
  }

  std::shared_ptr<PNGImage> LoadPNGImageShared(std::ifstream& stream) {
    std::shared_ptr<PNGImage> pngImg = std::make_shared<PNGImage>();
    bool result = LoadPNGImage(stream, *pngImg.get());
    return result ? pngImg : nullptr;
  }

}
