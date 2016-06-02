/*
 * neonGX - ShaderManager.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_SHADERMANAGER_H
#define NEONGX_SHADERMANAGER_H

#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/Shaders/TextureShader.hpp>
#include <neonGX/Core/Renderer/OpenGL/Shaders/FontTextureShader.hpp>
#include <memory>
#include <unordered_set>

namespace neonGX {

  enum class ShaderType {
    None,
    TextureShader,
    FontTextureShader
  };

  class ShaderManager {
  private:
    webgl_context_handle m_GlHandle;
    std::unique_ptr<TextureShader> m_TextureShader;
    std::unique_ptr<FontTextureShader> m_FontTextureShader;
    ShaderType m_CurrentShader = ShaderType::None;
    std::unordered_set<GLuint> m_ActivatedShaderAttributes;

  public:
    ShaderManager(webgl_context_handle glHandle) : m_GlHandle(glHandle) {
    }

    ~ShaderManager() = default;

    ShaderType GetCurrentShaderType() const {
      return m_CurrentShader;
    }

    void InitializeTextureShader() {
      if(m_TextureShader) {
        return;
      }
      m_TextureShader = std::make_unique<TextureShader>(m_GlHandle, this);
    }

    void InitializeFontTextureShader() {
      if(m_FontTextureShader) {
        return;
      }
      m_FontTextureShader = std::make_unique<FontTextureShader>(
          m_GlHandle, this);
    }

    void ActivateShaderAttributes() {
      GLShader* shader = nullptr;

      switch(m_CurrentShader) {
        case ShaderType::TextureShader:
          shader = m_TextureShader.get();
          break;
        case ShaderType::FontTextureShader:
          shader = m_FontTextureShader.get();
          break;
        default:
          assert("Invalid shader." && false);
          break;
      }

      for(auto& it : shader->m_shaderAttributes) {
        auto attribState = m_ActivatedShaderAttributes.find(it.second.Location);
        if(attribState == m_ActivatedShaderAttributes.end()) {
          m_ActivatedShaderAttributes.emplace(it.second.Location);
          glEnableVertexAttribArray(it.second.Location);
        }
      }
    }

    FontTextureShader& UseFontTextureShader() {
      if(m_CurrentShader != ShaderType::FontTextureShader) {
        InitializeFontTextureShader();

        m_CurrentShader = ShaderType::FontTextureShader;
        m_FontTextureShader->Bind();
      }

      return *m_FontTextureShader;
    }

    TextureShader& UseTextureShader() {
      if(m_CurrentShader != ShaderType::TextureShader) {
        InitializeTextureShader();

        m_CurrentShader = ShaderType::TextureShader;
        m_TextureShader->Bind();
      }

      return *m_TextureShader;
    }
  };

} // end namespace neonGX

#endif // !NEONGX_SHADERMANAGER_H
