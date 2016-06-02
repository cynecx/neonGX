/*
 * neonGX - FontTextureShader.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_FONTTEXTURESHADER_H
#define NEONGX_FONTTEXTURESHADER_H

#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Renderer/OpenGL/Shaders/GLShader.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <GSL/span.h>

namespace neonGX {

  class ShaderManager;

  class FontTextureShader : public GLShader {
  private:
    ShaderManager* m_ShaderManager;

  public:
    FontTextureShader(webgl_context_handle glHandle,
                      ShaderManager* shaderManager);
    virtual ~FontTextureShader();

    void SetProjectionMatrix(const Matrix3& mat);
  };

} // end namespace neonGX

#endif // !NEONGX_FONTTEXTURESHADER_H
