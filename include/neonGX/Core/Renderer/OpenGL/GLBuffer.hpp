/*
 * neonGX - GLBuffer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLBUFFER_H
#define NEONGX_GLBUFFER_H

#include <neonGX/Core/ADT.hpp>
#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Graphics/Color.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <vector>
#include <GSL/span.h>
#include <memory>

namespace neonGX {

  class GLBuffer {
  private:
    webgl_context_handle m_GLHandle;
    GLuint m_Buffer;
    int m_Type;
    int m_DrawType;

    bool m_Moved = false;

  public:
    GLBuffer(webgl_context_handle glHandle,
                int type = GL_ARRAY_BUFFER,
                int drawType = GL_STATIC_DRAW)
        : m_GLHandle(glHandle), m_Type(type), m_DrawType(drawType) {
      WebGLContextRAII switchCtx(glHandle);

      GLuint tmpBArr[1] = {0};
      glGenBuffers(1, tmpBArr);

      m_Buffer = tmpBArr[0];
    }

    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    GLBuffer(GLBuffer&& obj)
        : m_GLHandle(obj.m_GLHandle), m_Buffer(obj.m_Buffer),
          m_Type(obj.m_Type), m_DrawType(obj.m_DrawType) {
      obj.m_Moved = true;
    }

    GLBuffer& operator=(GLBuffer&& obj) {
      WebGLContextRAII switchCtx(m_GLHandle);

      GLuint tmpBArr[1] = { m_Buffer };
      glDeleteBuffers(1, tmpBArr);

      m_GLHandle = obj.m_GLHandle;
      m_Buffer = obj.m_Buffer;
      m_Type = obj.m_Type;
      m_DrawType = obj.m_DrawType;

      obj.m_Moved = true;

      return *this;
    }

    ~GLBuffer() {
      if(m_Moved) {
        return;
      }

      WebGLContextRAII switchCtx(m_GLHandle);

      GLuint tmpBArr[1] = { m_Buffer };
      glDeleteBuffers(1, tmpBArr);
    }

    void UploadData(gsl::span<const uint8_t> data, bool dontBind = false) {
      if(!dontBind) {
        Bind();
      }

      WebGLContextRAII switchCtx(m_GLHandle);
      glBufferData(m_Type, GLsizeiptr(data.size_bytes()), data.data(),
          m_DrawType);
    }

    void UploadSubData(gsl::span<const uint8_t> data, bool dontBind = false) {
      if(!dontBind) {
        Bind();
      }

      WebGLContextRAII switchCtx(m_GLHandle);
      glBufferSubData(m_Type, 0, GLsizeiptr(data.size_bytes()), data.data());
    }

    void Bind() {
      WebGLContextRAII switchCtx(m_GLHandle);
      glBindBuffer(m_Type, m_Buffer);
    }

    void Unbind() {
      WebGLContextRAII switchCtx(m_GLHandle);
      glBindBuffer(m_Type, 0);
    }

    static std::unique_ptr<GLBuffer> CreateVertexBuffer(
        webgl_context_handle glHandle, gsl::span<const uint8_t> data,
        int drawType) {

      std::unique_ptr<GLBuffer> buffer = std::make_unique<GLBuffer>(
          glHandle, GL_ARRAY_BUFFER, drawType);

      buffer->UploadData(data);

      return buffer;
    }

    static std::unique_ptr<GLBuffer> CreateIndexBuffer(
        webgl_context_handle glHandle, gsl::span<const uint8_t> data,
        int drawType) {

      std::unique_ptr<GLBuffer> buffer = std::make_unique<GLBuffer>(
          glHandle, GL_ELEMENT_ARRAY_BUFFER, drawType);

      buffer->UploadData(data);

      return buffer;
    }

    bool operator==(const GLBuffer& rhs) const {
      return (m_GLHandle == rhs.m_GLHandle) &&
          (m_Buffer == rhs.m_Buffer) &&
          (m_DrawType == rhs.m_DrawType) &&
          (m_Type == rhs.m_Type);
    }

    bool operator!=(const GLBuffer& rhs) const {
      return !(*this == rhs);
    }
  };

} // end namespace neonGX

#endif // !NEONGX_GLBUFFER_H
