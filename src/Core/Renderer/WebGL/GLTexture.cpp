/*
 * neonGX - GLTexture.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>

namespace neonGX {

  GLTexture::GLTexture(webgl_context_handle glHandle,
                       optional<FSize> size, GLenum format, GLenum type)
      : m_GLHandle(glHandle), m_Format(format), m_Type(type) {
    if(size) {
      m_Size = *size;
    }

    WebGLContextRAII switchCtx(glHandle);

    GLuint tmpTexArr[1] = {0};
    glGenTextures(1, tmpTexArr);

    m_Texture = tmpTexArr[0];
  }

  GLTexture::~GLTexture() {
    if(m_Moved) {
      return;
    }

    WebGLContextRAII switchCtx(m_GLHandle);

    GLuint tmpTexArr[1] = { m_Texture };
    glDeleteTextures(1, tmpTexArr);
  }

  void GLTexture::Bind(optional<GLenum> location) const {
    WebGLContextRAII switchCtx(m_GLHandle);

    if(location) {
      glActiveTexture(GL_TEXTURE0 + *location);
    }

    glBindTexture(GL_TEXTURE_2D, m_Texture);
  }

  void GLTexture::Unbind() {
    WebGLContextRAII switchCtx(m_GLHandle);
    glBindFramebuffer(GL_TEXTURE_2D, 0);
  }

  void GLTexture::UploadImage(const PNGImage& image) {
    Bind(nullopt);

    assert(m_Type == GL_UNSIGNED_BYTE);
    assert(m_Format == GL_RGBA);
    assert(image.RawData.size() >= (image.Width * image.Height * 4));

    WebGLContextRAII switchCtx(m_GLHandle);
    m_Size = FSize{ float(image.Width), float(image.Height) };

#ifdef NEONGX_USE_EMSCRIPTEN
    glPixelStorei(GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL, GL_TRUE);
#endif

    glTexImage2D(GL_TEXTURE_2D,
        0,
        m_Format,
        GLsizei(m_Size.width),
        GLsizei(m_Size.height),
        0,
        m_Format,
        m_Type,
        image.RawData.data());
  }

  void GLTexture::UploadData(gsl::span<const uint8_t> data,
                             optional<FSize> size) {
    Bind(nullopt);

    WebGLContextRAII switchCtx(m_GLHandle);

    if(size) {
      m_Size = *size;
    }

    assert(!m_Size.IsZero());

#ifdef NEONGX_USE_EMSCRIPTEN
    glPixelStorei(GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL, GL_TRUE);
#endif
    glTexImage2D(GL_TEXTURE_2D,
        0,
        m_Format,
        GLsizei(m_Size.width),
        GLsizei(m_Size.height),
        0,
        m_Format,
        m_Type,
        data.data());
  }

  FSize GLTexture::GetSize() const {
    return m_Size;
  }

  void GLTexture::SetMinFilter(ScaleMode mode) {
    WebGLContextRAII switchCtx(m_GLHandle);

    Bind(nullopt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        (mode == ScaleMode::Linear) ? GL_LINEAR : GL_NEAREST);
  }

  void GLTexture::SetMagFilter(ScaleMode mode) {
    WebGLContextRAII switchCtx(m_GLHandle);

    Bind(nullopt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        (mode == ScaleMode::Linear) ? GL_LINEAR : GL_NEAREST);
  }

  void GLTexture::EnableWrapClamp() {
    WebGLContextRAII switchCtx(m_GLHandle);

    Bind(nullopt);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  }

  void GLTexture::EnableWrapRepeat() {
    WebGLContextRAII switchCtx(m_GLHandle);

    Bind(nullopt);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

}
