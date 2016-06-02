/*
 * neonGX - GLRenderer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLRENDERER_H
#define NEONGX_GLRENDERER_H

#include <neonGX/Core/Renderer/Renderer.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLRenderTarget.hpp>
#include <neonGX/Core/Renderer/OpenGL/ObjectRenderer.hpp>
#include <neonGX/Core/Renderer/OpenGL/Managers/ShaderManager.hpp>
#include <neonGX/Core/Text/FontTextureManager.hpp>
#include <neonGX/Core/Display/DisplayObject.hpp>
#include <string>
#include <memory>

namespace neonGX {

  class SpriteRenderer;
  class TextRenderer;

  enum class ObjectRendererType {
    None,
    Sprite,
    Text
  };

  class GLRenderer final : public Renderer {
  public:
    webgl_context_handle webgl_handle;
    const std::string m_TargetView;

  private:
    friend class SpriteRenderer;
    friend class Sprite;
    friend class TextRenderer;
    friend class Text;

    std::unique_ptr<GLRenderTarget> m_RenderTarget;
    std::unique_ptr<ShaderManager> m_ShaderManager;
    std::unique_ptr<TextRenderer> m_TextRenderer;
    std::unique_ptr<SpriteRenderer> m_SpriteRenderer;
    std::shared_ptr<FontTextureManager> m_FontTextureManager;

    ObjectRendererType m_CurrentRendererType = ObjectRendererType::None;
    ObjectRenderer* m_CurrentRenderer = nullptr;

  public:
    explicit GLRenderer(const std::string& target, const FSize& size,
                        const RendererSettings& settings);
    virtual ~GLRenderer();

    void Render(DisplayObject* object) override;

    void Resize(const FSize& size) override;

    ObjectRenderer* SetObjectRenderer(ObjectRendererType type);

    std::shared_ptr<FontTextureManager> GetFontTextureManager();
  };

} // end namespace neonGX

#endif // !NEONGX_GLRENDERER_H
