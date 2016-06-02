/*
 * neonGX - TextureShader.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TEXTURESHADER_H
#define NEONGX_TEXTURESHADER_H

#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Renderer/OpenGL/Shaders/GLShader.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <GSL/span.h>

namespace neonGX {

  class ShaderManager;

  class TextureShader : public GLShader {
  private:
    GLint m_TextureCount = 1;
    ShaderManager* m_ShaderManager;

  public:
    TextureShader(webgl_context_handle glHandle,
                  ShaderManager* shaderManager);
    virtual ~TextureShader();

    void SetProjectionMatrix(const Matrix3& mat);
    void SetSampler2D(BaseTexture& data);
  };

} // end namespace neonGX

#endif // !NEONGX_TEXTURESHADER_H
