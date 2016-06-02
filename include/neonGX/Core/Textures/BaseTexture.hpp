/*
 * neonGX - BaseTexture.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_BASETEXTURE_H
#define NEONGX_BASETEXTURE_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Image/PNGImage.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>
#include <memory>
#include <unordered_map>

namespace neonGX {

  class GLTexture;

  enum class ScaleMode {
    Linear,
    Nearest
  };

  struct BaseTexture {
    float m_Resolution = 1;
    FSize m_Size{};
    ScaleMode m_ScaleMode = ScaleMode::Linear;
    std::shared_ptr<PNGImage> m_Image;

    std::unordered_map<webgl_context_handle, std::shared_ptr<GLTexture>>
        m_GlTextureMap;

    BaseTexture() = default;
    ~BaseTexture() = default;

    BaseTexture(const BaseTexture&) = default;
    BaseTexture& operator=(const BaseTexture&) = default;

    bool operator==(const BaseTexture& rhs) const {
      return m_Resolution == rhs.m_Resolution &&
          m_Size == rhs.m_Size &&
          m_ScaleMode == rhs.m_ScaleMode &&
          m_Image == rhs.m_Image;
    }

    bool operator!=(const BaseTexture& rhs) const {
      return !(*this == rhs);
    }

    bool IsPowerOfTwo() const {
      assert(m_Image.get() != nullptr);
      return m_Image->SizeIsPowerOfTwo();
    }

    void SetImage(std::shared_ptr<PNGImage> image);
    void Update();

    std::shared_ptr<GLTexture> GetGLTexture(webgl_context_handle glHandle);
  };

} // end namespace neonGX

#endif // !NEONGX_BASETEXTURE_H
