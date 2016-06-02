/*
 * neonGX - FontFontTextureShader.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/Shaders/FontTextureShader.hpp>
#include <neonGX/Core/Renderer/OpenGL/Managers/ShaderManager.hpp>

namespace neonGX {

  static constexpr auto VertexShaderSource =
      R"SOURCE(
          #version 100

          precision highp float;

          attribute vec2 aVertexPosition;
          attribute vec2 aTextureCoord;
          attribute vec3 aColor;

          uniform mat3 projectionMatrix;

          varying vec2 vTextureCoord;
          varying vec3 vColor;

          void main(void){
             gl_Position = vec4((projectionMatrix * vec3(aVertexPosition, 1.0)).xy, 0.0, 1.0);
             vTextureCoord = aTextureCoord;
             vColor = aColor;
          }
        )SOURCE";

  static constexpr auto FragmentShaderSource =
      R"SOURCE(
          #version 100

          precision highp float;

          varying vec2 vTextureCoord;
          varying vec3 vColor;

          uniform sampler2D uSampler;

          void main() {
            gl_FragColor = texture2D(uSampler, vTextureCoord) * vec4(vColor, 1.0);
          }
        )SOURCE";

  FontTextureShader::FontTextureShader(webgl_context_handle glHandle,
                                       ShaderManager* shaderManager)
      : GLShader(glHandle, VertexShaderSource, FragmentShaderSource),
        m_ShaderManager(shaderManager) {
  }

  FontTextureShader::~FontTextureShader() = default;

  void FontTextureShader::SetProjectionMatrix(const Matrix3& mat) {
    if(m_ShaderManager) {
      m_ShaderManager->UseFontTextureShader();
    } else {
      GLShader::Bind();
    }

    WebGLContextRAII switchCtx(m_GLHandle);

    const auto col0 = mat.GetColumnAccessor(0);
    const auto col1 = mat.GetColumnAccessor(1);
    const auto col2 = mat.GetColumnAccessor(2);

    float matTemp[] {
        col0[0], col0[1], col0[2],
        col1[0], col1[1], col1[2],
        col2[0], col2[1], col2[2]
    };

    GLint location = m_uniformLocations["projectionMatrix"].Location;
    glUniformMatrix3fv(location, 1, GL_FALSE, matTemp);
  }

}
