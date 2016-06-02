/*
 * neonGX - Sprite.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_SPRITE_H
#define NEONGX_SPRITE_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Display/Container.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLRenderer.hpp>
#include <array>
#include <cmath>
#include <memory>

namespace neonGX {

  class Sprite final : public Container {
  public:
    friend class SpriteRenderer;

  private:
    bool m_TextureDirty = true;

    FPoint m_Anchor;
    FSize m_CurrSize;

    std::shared_ptr<Texture> m_Texture;

    std::array<float, 8> m_VertexData;
    uint32_t m_Tint = 0xFFFFFF;

  public:
    Sprite(const std::shared_ptr<Texture>& texture);
    virtual ~Sprite();

    FPoint GetAnchor() const {
      return m_Anchor;
    }

    std::shared_ptr<Texture> GetTexture() {
      return m_Texture;
    }

    void SetAnchor(const FPoint& point) {
      m_TextureDirty = true;
      m_Anchor = point;
    }

    FSize GetSize() const {
      auto textureSize = m_Texture->GetSize();
      return {
          std::abs(m_Scale.x) * textureSize.width,
          std::abs(m_Scale.y) * textureSize.height,
      };
    }

    void SetSize(const FSize& size) {
      auto textureSize = m_Texture->GetSize();
      m_Scale.x =
          ExtractSign(m_Scale.x) * size.width / textureSize.width;
      m_Scale.y =
          ExtractSign(m_Scale.y) * size.height / textureSize.height;
      m_CurrSize = size;
    }

    void SetTexture(const std::shared_ptr<Texture>& texture);
    void CalculateVertices();

    void OnStateChanged() override {
      Container::OnStateChanged();
      m_TextureDirty = true;
    }

    void RenderWebGL(GLRenderer* renderer) override;
   };

} // end namespace neonGX

#endif // !NEONGX_SPRITE_H
