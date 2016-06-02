/*
 * neonGX - GLShader.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/Shaders/GLShader.hpp>

namespace neonGX {

  GLShader::GLShader(webgl_context_handle glHandle, const std::string& vertexSrc,
                 const std::string& fragmentSrc) :
      m_GLHandle(glHandle), m_VertexSrc(vertexSrc), m_FragmentSrc(fragmentSrc) {
  }

  GLShader::~GLShader() = default;

  void GLShader::Bind() {
    if(m_Program == 0) {
      InitializeShaders();
    }

    assert(m_Program != 0);

    WebGLContextRAII switchCtx(m_GLHandle);
    glUseProgram(m_Program);
  }

  void GLShader::CompileShaders() {
    auto compileShader = [] (GLenum type, const std::string& str) -> GLuint {
      auto shader = glCreateShader(type);

      const char* sourceArr[] = { str.c_str() };
      int sourceSizeArr[] = { int(str.size()) };

      glShaderSource(shader, 1, sourceArr, sourceSizeArr);
      glCompileShader(shader);

      GLint status = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

      if(status == GL_FALSE) {
        char log[1024 * 4] = {0};
        GLsizei outWritten = 0;
        glGetShaderInfoLog(shader, sizeof(log) - 1, &outWritten, log);
        std::cout << log << std::endl;
      }

      assert(status == GL_TRUE);

      return shader;
    };

    WebGLContextRAII switchCtx(m_GLHandle);

    auto vertexShader = compileShader(GL_VERTEX_SHADER, m_VertexSrc);
    auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, m_FragmentSrc);

    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    assert(status == GL_TRUE);

    m_Program = program;
  }

  void GLShader::InitializeShaders() {
    CompileShaders();

    WebGLContextRAII switchCtx(m_GLHandle);
    glUseProgram(m_Program);

    GLint activeAttributes = 0;
    glGetProgramiv(m_Program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);

    for(GLsizei i = 0; i < activeAttributes; i++) {
      GLchar strBuffer[255] = { 0 };
      GLsizei outSize = 0;
      GLsizei outWritten = 0;
      GLenum outType = 0;

      glGetActiveAttrib(m_Program, i, sizeof(strBuffer) - 1, &outWritten,
          &outSize, &outType, strBuffer);

      assert(outWritten > 0);
      assert(size_t(outWritten) < sizeof(strBuffer));

      std::string strAttribute(strBuffer);

      m_shaderAttributes.emplace(strAttribute, VertexShaderAttributeInfo{
          GLuint(glGetAttribLocation(m_Program, strBuffer)),
          strAttribute, outSize, outType
      });
    }

    GLint activeUniforms = 0;
    glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &activeUniforms);

    for(GLsizei i = 0; i < activeUniforms; i++) {
      GLchar strBuffer[255] = { 0 };
      GLsizei outSize = 0;
      GLsizei outWritten = 0;
      GLenum outType = 0;

      glGetActiveUniform(m_Program, i, sizeof(strBuffer) - 1, &outWritten,
          &outSize, &outType, strBuffer);

      assert(outWritten > 0);
      assert(size_t(outWritten) < sizeof(strBuffer));

      std::string strUniform(strBuffer);
      m_uniformLocations.emplace(strUniform, VertexShaderUniformInfo{
          glGetUniformLocation(m_Program, strBuffer),
          strUniform, outType
      });
    }
  }

}
