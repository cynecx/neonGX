/*
 * neonGX - BaseTexture.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Textures/BaseTexture.hpp>

namespace neonGX {

  void BaseTexture::SetImage(std::shared_ptr<PNGImage> image) {
    m_Image = image;
    Update();
  }

  void BaseTexture::Update() {
    assert(m_Image.get() != nullptr);

    m_Size = {
        float(m_Image->Width) / m_Resolution,
        float(m_Image->Height) / m_Resolution
    };
  }

  std::shared_ptr<GLTexture> BaseTexture::GetGLTexture(
      webgl_context_handle glHandle) {
    auto it = m_GlTextureMap.find(glHandle);
    if(it == m_GlTextureMap.end()) {
      std::shared_ptr<GLTexture> texture = std::make_shared<GLTexture>(
          glHandle, nullopt);
      m_GlTextureMap.insert({glHandle, texture});

      texture->UploadImage(*m_Image.get());

      texture->SetMinFilter(m_ScaleMode);
      texture->SetMagFilter(m_ScaleMode);

      if(IsPowerOfTwo()) {
        texture->EnableWrapRepeat();
      } else {
        texture->EnableWrapClamp();
      }

      return texture;
    } else {
      return it->second;
    }
  }


}
