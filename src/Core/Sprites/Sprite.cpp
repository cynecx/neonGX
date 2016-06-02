/*
 * neonGX - Sprite.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Sprites/Sprite.hpp>
#include <neonGX/Core/Sprites/SpriteRenderer.hpp>

namespace neonGX {

  Sprite::Sprite(const std::shared_ptr<Texture>& texture) {
    assert(texture.get() != nullptr);
    SetTexture(texture);
  }

  Sprite::~Sprite() = default;

  void Sprite::RenderWebGL(GLRenderer *renderer) {
    if(m_TextureDirty) {
      m_TextureDirty = false;
      CalculateVertices();
    }

    renderer->SetObjectRenderer(ObjectRendererType::Sprite)->Render(this);
  }

  void Sprite::SetTexture(const std::shared_ptr<Texture>& texture) {
    if(!texture || texture == m_Texture) {
      return;
    }

    m_Texture = texture;
    m_TextureDirty = true;

    if(m_CurrSize.width) {
      m_Scale.x = ExtractSign(m_Scale.x) *
                  m_CurrSize.width / m_Texture->GetSize().width;
    }

    if(m_CurrSize.height) {
      m_Scale.y = ExtractSign(m_Scale.y) *
                  m_CurrSize.height / m_Texture->GetSize().height;
    }
  }

  void Sprite::CalculateVertices() {
    FRectangle orig = m_Texture->GetFrame();

    float w0 = (orig.size.width) * (1 - m_Anchor.x);
    float w1 = (orig.size.width) * -m_Anchor.x;

    float h0 = orig.size.height * (1 - m_Anchor.y);
    float h1 = orig.size.height * -m_Anchor.y;

    auto result = m_WorldTransform * Matrix<3, 1, float>{w1, h1, 1.0f};
    auto accessor = result.GetColumnAccessor(0);
    m_VertexData[0] = accessor[0];
    m_VertexData[1] = accessor[1];

    result = m_WorldTransform * Matrix<3, 1, float>{w0, h1, 1.0f};
    auto accessor2 = result.GetColumnAccessor(0);
    m_VertexData[2] = accessor2[0];
    m_VertexData[3] = accessor2[1];

    result = m_WorldTransform * Matrix<3, 1, float>{w0, h0, 1.0f};
    auto accessor3 = result.GetColumnAccessor(0);
    m_VertexData[4] = accessor3[0];
    m_VertexData[5] = accessor3[1];

    result = m_WorldTransform * Matrix<3, 1, float>{w1, h0, 1.0f};
    auto accessor4 = result.GetColumnAccessor(0);
    m_VertexData[6] = accessor4[0];
    m_VertexData[7] = accessor4[1];
  }

}
