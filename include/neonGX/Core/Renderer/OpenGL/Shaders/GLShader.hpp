/*
 * neonGX - GLShader.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLSHADER_H
#define NEONGX_GLSHADER_H

#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <cassert>
#include <unordered_map>
#include <string>
#include <iostream>

namespace neonGX {

  struct VertexShaderAttributeInfo {
    GLuint Location;
    std::string Name;
    GLsizei Size;
    GLenum Type;
  };

  struct VertexShaderUniformInfo {
    GLint Location;
    std::string Name;
    GLenum Type;
  };

  class GLShader {
  protected:
    webgl_context_handle m_GLHandle;

    std::string m_VertexSrc;
    std::string m_FragmentSrc;

    GLuint m_Program = 0;

  public:
    std::unordered_map<std::string, VertexShaderAttributeInfo>
        m_shaderAttributes;
    std::unordered_map<std::string, VertexShaderUniformInfo>
        m_uniformLocations;

  public:
    GLShader(webgl_context_handle glHandle, const std::string& vertexSrc,
           const std::string& fragmentSrc);

    GLShader(const GLShader&) = delete;
    GLShader& operator=(const GLShader&) = delete;

    virtual ~GLShader();

    void Bind();

  private:
    void CompileShaders();
    void InitializeShaders();
  };

} // end namespace neonGX

#endif // !NEONGX_GLSHADER_H
