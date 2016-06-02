/*
 * neonGX - Renderer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_RENDERER_H
#define NEONGX_RENDERER_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Graphics/Color.hpp>

namespace neonGX {

  class DisplayObject;

  struct RendererSettings {
    bool Transparent = false;
    bool ClearBeforeRender = true;
    ColorRGB BackgroundColor{};
    int Resolution = 1;
  };

  class Renderer {
  protected:
    FSize m_Size;
    RendererSettings m_Settings;

  public:
    Renderer() = default;

    Renderer(const FSize& size, const RendererSettings& settings) :
        m_Size{size}, m_Settings(settings) {
    }

    virtual ~Renderer();

    virtual void Render(DisplayObject* object) = 0;

    virtual void Resize(const FSize& size) {
      m_Size = FSize(
          size.width * m_Settings.Resolution,
          size.height * m_Settings.Resolution);
    }
  };

} // end namespace neonGX

#endif // !NEONGX_RENDERER_H
