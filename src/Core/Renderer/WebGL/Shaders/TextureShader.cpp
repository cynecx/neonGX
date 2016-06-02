/*
 * neonGX - TextureShader.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/Shaders/TextureShader.hpp>
#include <neonGX/Core/Renderer/OpenGL/Managers/ShaderManager.hpp>

namespace neonGX {

  static constexpr auto VertexShaderSource =
      R"SOURCE(
          #version 100

          precision highp float;

          attribute vec2 aVertexPosition;
          attribute vec2 aTextureCoord;
          attribute vec4 aColor;

          uniform mat3 projectionMatrix;

          varying vec2 vTextureCoord;
          varying vec4 vColor;

          void main(void){
             gl_Position = vec4((projectionMatrix * vec3(aVertexPosition, 1.0)).xy, 0.0, 1.0);
             vTextureCoord = aTextureCoord;
             vColor = vec4(aColor.rgb * aColor.a, aColor.a);
          }
        )SOURCE";

  static constexpr auto FragmentShaderSource =
      R"SOURCE(
          #version 100

          precision highp float;

          varying vec2 vTextureCoord;
          varying vec4 vColor;

          uniform sampler2D uSampler;

          void main() {
            gl_FragColor = texture2D(uSampler, vTextureCoord) * vColor;
          }
        )SOURCE";

  TextureShader::TextureShader(webgl_context_handle glHandle,
                               ShaderManager* shaderManager)
      : GLShader(glHandle, VertexShaderSource, FragmentShaderSource),
        m_ShaderManager(shaderManager) {
  }

  TextureShader::~TextureShader() = default;

  void TextureShader::SetProjectionMatrix(const Matrix3& mat) {
    if(m_ShaderManager) {
      m_ShaderManager->UseTextureShader();
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

  void TextureShader::SetSampler2D(BaseTexture& data) {
    if(m_ShaderManager) {
      m_ShaderManager->UseTextureShader();
    } else {
      GLShader::Bind();
    }

    WebGLContextRAII switchCtx(m_GLHandle);

    glActiveTexture(GLenum(GL_TEXTURE0 + m_TextureCount));
    data.GetGLTexture(m_GLHandle)->Bind(nullopt);

    GLint location = m_uniformLocations["uSampler"].Location;
    glUniform1i(location, m_TextureCount);

    m_TextureCount++;
  }

}
