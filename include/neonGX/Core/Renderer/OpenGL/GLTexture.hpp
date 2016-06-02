/*
 * neonGX - GLTexture.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLTEXTURE_H
#define NEONGX_GLTEXTURE_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Graphics/Color.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Image/PNGImage.hpp>
#include <neonGX/Core/ADT.hpp>
#include <GSL/span.h>
#include <neonGX/Core/Textures/BaseTexture.hpp>

namespace neonGX {

  enum class ScaleMode;

  class GLTexture {
  public:
    friend class GLFrameBuffer;

  private:
    webgl_context_handle m_GLHandle;
    GLuint m_Texture;
    FSize m_Size{0.0f, 0.0f};
    GLenum m_Format;
    GLenum m_Type;

    bool m_Moved = false;

  public:
    GLTexture(webgl_context_handle glHandle, optional<FSize> size,
                 GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

    GLTexture(const GLTexture&) = delete;
    GLTexture& operator=(const GLTexture&) = delete;

    GLTexture(GLTexture&& obj)
        : m_GLHandle(obj.m_GLHandle), m_Texture(obj.m_Texture),
          m_Size(obj.m_Size), m_Format(obj.m_Format), m_Type(obj.m_Type) {
      obj.m_Moved = true;
    }

    GLTexture& operator=(GLTexture&& obj) {
      WebGLContextRAII switchCtx(m_GLHandle);

      GLuint tmpTexArr[1] = { m_Texture };
      glDeleteTextures(1, tmpTexArr);

      m_GLHandle = obj.m_GLHandle;
      m_Texture = obj.m_Texture;
      m_Size = obj.m_Size;
      m_Format = obj.m_Format;
      m_Type = obj.m_Type;

      obj.m_Moved = true;

      return *this;
    }

    ~GLTexture();

    void Bind(optional<GLenum> location) const;
    void Unbind();

    void UploadImage(const PNGImage& image);
    void UploadData(gsl::span<const uint8_t> data, optional<FSize> size);

    FSize GetSize() const;

    void SetMinFilter(ScaleMode mode);
    void SetMagFilter(ScaleMode mode);
    void EnableWrapClamp();
    void EnableWrapRepeat();
  };

} // end namespace neonGX

#endif // !NEONGX_GLTEXTURE_H
