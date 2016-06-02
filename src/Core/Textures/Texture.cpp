/*
 * neonGX - Texture.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Textures/Texture.hpp>

namespace neonGX {

  void TextureUVs::Set(const FRectangle& frame, const FSize& baseFrame) {
    auto tw = baseFrame.width;
    auto th = baseFrame.height;

    P0.x = frame.point.x / tw;
    P0.y = frame.point.y / th;

    P1.x = (frame.point.x + frame.size.width) / tw;
    P1.y = frame.point.y / th;

    P2.x = (frame.point.x + frame.size.width) / tw;
    P2.y = (frame.point.y + frame.size.height) / th;

    P3.x = frame.point.x / tw;
    P3.y = (frame.point.y + frame.size.height) / th;
  }

  Texture::Texture(std::shared_ptr<BaseTexture> baseTexture,
                   optional<FRectangle> frame)
      : m_BaseTexture(baseTexture) {
    m_UVs = TextureUVs{};
    if(frame) {
      SetFrame(frame.value());
    } else {
      SetFrame(FRectangle{ {0, 0}, m_BaseTexture->m_Size });
    }
  }

  void Texture::SetFrame(const FRectangle& frame) {
    m_Frame = frame;

    if(frame.point.x + frame.size.width > m_BaseTexture->m_Size.width ||
       frame.point.y + frame.size.height > m_BaseTexture->m_Size.height) {
      assert("Frame is invalid" && false);
    }

    m_Orig = m_Frame;
    m_IsValid = frame.size > FSize{0.0f, 0.0f};

    if(m_IsValid) {
      m_UVs->Set(m_Frame, m_BaseTexture->m_Size);
    }
  }

}
