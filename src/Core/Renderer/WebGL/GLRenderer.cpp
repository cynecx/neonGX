/*
 * neonGX - GLRenderer.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/GLRenderer.hpp>
#include <neonGX/Core/Sprites/SpriteRenderer.hpp>
#include <neonGX/Core/Text/TextRenderer.hpp>

#ifdef NEONGX_USE_EMSCRIPTEN
#include <emscripten/val.h>
#endif

#include <boost/lexical_cast.hpp>

namespace neonGX {

  GLRenderer::GLRenderer(const std::string& target, const FSize& size,
                const RendererSettings& settings)
      : Renderer(size, settings), m_TargetView(target) {
#ifdef NEONGX_USE_EMSCRIPTEN
    EmscriptenWebGLContextAttributes gl_settings{};

      gl_settings.alpha = EM_BOOL(false);
      gl_settings.depth = EM_BOOL(false);
      gl_settings.stencil = EM_BOOL(true);
      gl_settings.antialias = EM_BOOL(false);
      gl_settings.premultipliedAlpha = EM_BOOL(false);
      gl_settings.preferLowPowerToHighPerformance = EM_BOOL(false);
      gl_settings.failIfMajorPerformanceCaveat = EM_BOOL(false);
      gl_settings.majorVersion = 1;
      gl_settings.minorVersion = 0;
      gl_settings.enableExtensionsByDefault = false;

      webgl_handle = emscripten_webgl_create_context(
          target.c_str(), &gl_settings);
      assert(webgl_handle > 0 && "Invalid WebGL handle");
#else
    webgl_handle = glfwCreateWindow(int(size.width), int(size.width),
        "neonGX", nullptr, nullptr);
#endif
    WebGLContextRAII switchCtx(webgl_handle);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_RenderTarget = std::make_unique<GLRenderTarget>(
        webgl_handle, size, settings.Resolution, ScaleMode::Linear, true);

    Resize(size);

    m_FontTextureManager = std::make_shared<FontTextureManager>(webgl_handle);
    m_ShaderManager = std::make_unique<ShaderManager>(webgl_handle);
    m_SpriteRenderer = std::make_unique<SpriteRenderer>(this);
    m_TextRenderer = std::make_unique<TextRenderer>(this);
  }

  GLRenderer::~GLRenderer() {
#ifndef NEONGX_USE_EMSCRIPTEN
    glfwDestroyWindow(webgl_handle);
#endif
  }

  void GLRenderer::Render(DisplayObject* object) {
    assert(object != nullptr);

    WebGLContextRAII switchCtx(webgl_handle);

    object->UpdateTransform(true);

    if(m_Settings.ClearBeforeRender) {
      if(m_Settings.Transparent) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      } else {
        ColorRGBA rgba(m_Settings.BackgroundColor);
        glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);
      }
      glClear(GL_COLOR_BUFFER_BIT);
    }

    object->RenderWebGL(this);
    m_CurrentRenderer->Flush();
  }

#ifdef NEONGX_USE_EMSCRIPTEN
  static void EmscriptenResizeElement(const std::string& elementId,
      const FSize& size) {
    using namespace emscripten;
    val document = val::global("document");
    val canvasObj = document.call<val>("getElementById", elementId);
    canvasObj.call<void>("setAttribute",
        std::string("width"),  boost::lexical_cast<std::string>(size.width));
    canvasObj.call<void>("setAttribute",
        std::string("height"),  boost::lexical_cast<std::string>(size.height));
  }
#endif

  void GLRenderer::Resize(const FSize& size) {
    Renderer::Resize(size);

#ifdef NEONGX_USE_EMSCRIPTEN
    EmscriptenResizeElement(m_TargetView, m_Size);
#else
    glfwSetWindowSize(webgl_handle, int(m_Size.width), int(m_Size.width));
#endif

    WebGLContextRAII switchCtx(webgl_handle);

    m_RenderTarget->Resize(m_Size);
    m_RenderTarget->Activate();
  }

  ObjectRenderer* GLRenderer::SetObjectRenderer(ObjectRendererType type) {
    if(m_CurrentRendererType == type) {
      return m_CurrentRenderer;
    }

    if(m_CurrentRenderer) {
      m_CurrentRenderer->Stop();
    }

    switch(type) {
      case ObjectRendererType::Sprite:
        m_CurrentRenderer = m_SpriteRenderer.get();
        break;
      case ObjectRendererType::Text:
        m_CurrentRenderer = m_TextRenderer.get();
        break;
      default:
        assert("Invalid ObjectRenderer type." && false);
        break;
    }

    m_CurrentRendererType = type;
    assert(m_CurrentRenderer != nullptr);
    m_CurrentRenderer->Start();

    return m_CurrentRenderer;
  }

  std::shared_ptr<FontTextureManager> GLRenderer::GetFontTextureManager() {
    return m_FontTextureManager;
  }

}
