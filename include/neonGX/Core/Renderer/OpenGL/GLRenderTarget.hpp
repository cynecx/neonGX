/*
 * neonGX - GLRenderTarget.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLRENDERTARGET_H
#define NEONGX_GLRENDERTARGET_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Math/Helpers.hpp>
#include <neonGX/Core/Math/Transformation.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLFrameBuffer.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>
#include <neonGX/Core/Textures/BaseTexture.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <neonGX/Core/ADT.hpp>
#include <memory>

namespace neonGX {

  class GLRenderTarget {
  public:
    webgl_context_handle m_GLHandle;

    FRectangle m_Size{ {0, 0}, {1, 1} };
    float m_Resolution;

    Matrix3 m_ProjectionMatrix;
    optional<Matrix3> m_TransformMatrix;

    ScaleMode m_ScaleMode;
    bool m_Root;

    std::unique_ptr<GLFrameBuffer> m_FrameBuffer;

  public:
    GLRenderTarget(webgl_context_handle glHandle, const FSize& size,
                      float resolution, ScaleMode scaleMode, bool root)
        : m_GLHandle(glHandle), m_Resolution(resolution),
          m_ScaleMode(scaleMode), m_Root(root) {
      WebGLContextRAII switchCtx(glHandle);

      if(!m_Root) {
        m_FrameBuffer = GLFrameBuffer::CreateRGBA(
            glHandle, FSize{100.0f, 100.0f}, m_ScaleMode);
      }

      Resize(size);
    }

    ~GLRenderTarget() {

    }

    void Clear(ColorRGBA rgba = {0, 0, 0, 0}) {
      WebGLContextRAII switchCtx(m_GLHandle);
      if(m_FrameBuffer) {
        m_FrameBuffer->Clear(rgba);
      }
    }

    void Activate() {
      WebGLContextRAII switchCtx(m_GLHandle);

      if(m_FrameBuffer) {
        m_FrameBuffer->Bind();
      }

      CalculateProjection(m_Size, nullopt);

      if(m_TransformMatrix) {
        m_ProjectionMatrix *= m_TransformMatrix.value();
      }

      glViewport(
          GLint(m_Size.point.x),
          GLint(m_Size.point.y),
          GLint(m_Size.size.width * m_Resolution),
          GLint(m_Size.size.height * m_Resolution));
    }

    void CalculateProjection(const FRectangle& destFrame,
                             optional<FPoint> srcFrame) {
      m_ProjectionMatrix = GetToClipspaceTransform(
          destFrame.size,
          srcFrame ? srcFrame.value() : destFrame.point,
          m_Root);
    }

    void Resize(const FSize& size) {
      if(m_Size.size == size) {
        return;
      }

      m_Size.size = size;

      if(!m_Root) {
        m_FrameBuffer->Resize(
            FSize{ size.width * m_Resolution, size.height * m_Resolution });
      }

      CalculateProjection(m_Size, nullopt);
    }
  };

} // end namespace neonGX

#endif // !NEONGX_GLRENDERTARGET_H
