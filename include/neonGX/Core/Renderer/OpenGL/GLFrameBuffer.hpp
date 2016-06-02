/*
 * neonGX - GLFrameBuffer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLFRAMEBUFFER_H
#define NEONGX_GLFRAMEBUFFER_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Graphics/Color.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <GSL/span.h>
#include <memory>
#include <neonGX/Core/ADT.hpp>

namespace neonGX {

  class GLFrameBuffer {
  private:
    webgl_context_handle m_GLHandle;
    optional<GLuint> m_FrameBuffer;
    FSize m_Size;

  public:
    std::shared_ptr<GLTexture> m_Texture;

  public:
    GLFrameBuffer(webgl_context_handle glHandle, const FSize& size)
        : m_GLHandle(glHandle), m_Size(size) {
      WebGLContextRAII switchCtx(glHandle);

      GLuint tmpFBArr[1] = {0};
      glGenFramebuffers(1, tmpFBArr);

      m_FrameBuffer = tmpFBArr[0];
    }

    GLFrameBuffer(const GLFrameBuffer&) = delete;
    GLFrameBuffer& operator=(const GLFrameBuffer&) = delete;

    GLFrameBuffer(GLFrameBuffer&&) = delete;
    GLFrameBuffer& operator=(GLFrameBuffer&&) = delete;

    ~GLFrameBuffer() {
      if(!m_FrameBuffer) {
        return;
      }

      WebGLContextRAII switchCtx(m_GLHandle);

      GLuint tmpFBArr[1] = { m_FrameBuffer.value() };
      glDeleteFramebuffers(1, tmpFBArr);
    }

    void Bind() {
      WebGLContextRAII switchCtx(m_GLHandle);
      glBindFramebuffer(GL_FRAMEBUFFER,
          m_FrameBuffer ? m_FrameBuffer.value() : 0);
    }

    void Unbind() {
      WebGLContextRAII switchCtx(m_GLHandle);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void EnableTexture(std::shared_ptr<GLTexture> texture) {
      assert(texture.get() != nullptr);
      m_Texture = texture;

      WebGLContextRAII switchCtx(m_GLHandle);

      m_Texture->Bind(nullopt);
      Bind();

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_2D, m_Texture->m_Texture, 0);
    }

    void Resize(const FSize& size) {
      WebGLContextRAII switchCtx(m_GLHandle);
      m_Size = size;

      if(m_Texture) {
        m_Texture->UploadData({nullptr}, m_Size);
      }
    }

    void Clear(const ColorRGBA& color) {
      Bind();

      WebGLContextRAII switchCtx(m_GLHandle);
      glClearColor(color.r, color.g, color.b, color.a);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    static std::unique_ptr<GLFrameBuffer> CreateRGBA(
        webgl_context_handle glHandle, const FSize& size, ScaleMode scaleMode) {
      std::shared_ptr<GLTexture> texture(
          std::make_shared<GLTexture>(glHandle, size));

      texture->SetMagFilter(scaleMode);
      texture->SetMinFilter(scaleMode);

      if(size.IsIntegral() &&
          IsPowerOfTwo(GetIntegralFromFP<uint32_t>(size.width)) &&
          IsPowerOfTwo(GetIntegralFromFP<uint32_t>(size.height))) {
        texture->EnableWrapRepeat();
      } else {
        texture->EnableWrapClamp();
      }

      std::unique_ptr<GLFrameBuffer> frameBuffer =
          std::make_unique<GLFrameBuffer>(glHandle, size);
      frameBuffer->EnableTexture(texture);
      frameBuffer->Unbind();

      return frameBuffer;
    }
  };

} // end namespace neonGX

#endif // !NEONGX_GLFRAMEBUFFER_H
