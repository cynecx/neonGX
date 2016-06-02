/*
 * neonGX - Text.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Text/Text.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLRenderer.hpp>
#include <neonGX/Core/Text/TextRenderer.hpp>
#include <neonGX/Core/Math/Primitives.hpp>

namespace neonGX {

  Text::Text(const std::string& text, const std::string& fontName) :
      m_Text(text), m_FontName(fontName) {
  }

  Text::~Text() = default;

  FSize Text::GetSize(GLRenderer* renderer) const {
    assert(renderer != nullptr);

    auto optFontData = renderer->GetFontTextureManager()->GetFontData(
        m_FontName);
    if(!optFontData) {
      return {0.0f, 0.0f};
    }

    const FontData* fontData = optFontData.value();

    FSize retSize;

    for(size_t i = 0; i < m_Text.size(); i++) {
      char16_t characterToRender = char16_t(m_Text[i]);

      if(characterToRender >= 128) {
        characterToRender = 0;
      }

      auto it = fontData->CharTextures.find(characterToRender);
      const auto& glyphInfo = it->second;
      FSize glyphSize = glyphInfo.Texture.GetSize();

      retSize.width += glyphInfo.AdvanceX >> 6;
      retSize.height = std::max(retSize.height, glyphSize.height);
    }

    return retSize;
  }

  void Text::CalculateVertices(GLRenderer* renderer) {
    assert(renderer != nullptr);

    auto optFontData = renderer->m_FontTextureManager->GetFontData(m_FontName);
    if(!optFontData) {
      return;
    }

    size_t vertexCount = 8 * m_Text.size();
    m_VertexData.resize(vertexCount);

    const FontData* fontData = optFontData.value();

    float currentX = 0;

    for(size_t i = 0; i < m_Text.size(); i++) {
      float* vertexData = m_VertexData.data() + i * 8;
      char16_t characterToRender = char16_t(m_Text[i]);

      if(characterToRender >= 128) {
        characterToRender = 0;
      }

      auto it = fontData->CharTextures.find(characterToRender);
      const auto& glyphInfo = it->second;

      FSize glyphSize = glyphInfo.Texture.GetSize();

      if(!glyphSize.IsZero()) {
        float w1 = currentX + float(glyphInfo.BearingX);
        float w0 = w1 + glyphSize.width;

        float h1 = glyphInfo.DescentY - glyphInfo.BearingY;
        float h0 = glyphSize.height + h1;

        auto result = m_WorldTransform * Matrix<3, 1, float>{w1, h1, 1.0f};
        auto accessor = result.GetColumnAccessor(0);
        vertexData[0] = accessor[0];
        vertexData[1] = accessor[1];

        result = m_WorldTransform * Matrix<3, 1, float>{w0, h1, 1.0f};
        auto accessor2 = result.GetColumnAccessor(0);
        vertexData[2] = accessor2[0];
        vertexData[3] = accessor2[1];

        result = m_WorldTransform * Matrix<3, 1, float>{w0, h0, 1.0f};
        auto accessor3 = result.GetColumnAccessor(0);
        vertexData[4] = accessor3[0];
        vertexData[5] = accessor3[1];

        result = m_WorldTransform * Matrix<3, 1, float>{w1, h0, 1.0f};
        auto accessor4 = result.GetColumnAccessor(0);
        vertexData[6] = accessor4[0];
        vertexData[7] = accessor4[1];
      } else {
        m_ExcludeSet.emplace(i);
      }

      currentX += (glyphInfo.AdvanceX >> 6);
    }
  }

  void Text::RenderWebGL(GLRenderer* renderer) {
    assert(renderer != nullptr);

    if(m_IsDirty) {
      m_IsDirty = false;
      CalculateVertices(renderer);
    }

    if(m_VertexData.empty()) {
      return;
    }

    renderer->SetObjectRenderer(ObjectRendererType::Text)->Render(this);
  }

}
