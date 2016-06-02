/*
 * neonGX - Text.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TEXT_H
#define NEONGX_TEXT_H

#include <neonGX/Core/Display/DisplayObject.hpp>
#include <neonGX/Core/Graphics/Color.hpp>
#include <string>
#include <unordered_set>
#include <vector>

namespace neonGX {

  class Text : public DisplayObject {
  public:
    friend class TextRenderer;

  private:
    std::string m_Text;
    std::string m_FontName;

    bool m_IsDirty = true;
    bool m_HasValidFontData = false;

    std::vector<float> m_VertexData;
    std::unordered_set<size_t> m_ExcludeSet;

    ColorRGB m_Color{0, 0, 0};

  public:
    explicit Text(const std::string& text, const std::string& fontName);
    virtual ~Text() override;

    const std::string& GetText() const {
      return m_Text;
    }

    void SetText(const std::string& text) {
      m_Text = text;
      m_IsDirty = true;
    }

    ColorRGB& GetColorRef() {
      return m_Color;
    }

    void OnStateChanged() override {
      m_IsDirty = true;
    }

    FSize GetSize(GLRenderer* renderer) const;

    void CalculateVertices(GLRenderer* renderer);

    using DisplayObject::UpdateTransform;

    void RenderWebGL(GLRenderer* renderer) override;
  };

} // end namespace neonGX

#endif // !NEONGX_TEXT_H
