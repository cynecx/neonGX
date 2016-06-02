/*
 * neonGX - TextureManager.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TEXTUREMANAGER_H
#define NEONGX_TEXTUREMANAGER_H

#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>

namespace neonGX {

  class TextureManager {
  private:
    webgl_context_handle m_GlHandle;
    GLuint m_CurrentTexture = 0;

  public:
    TextureManager(webgl_context_handle glHandle) : m_GlHandle(glHandle) {
      WebGLContextRAII switchCtx(glHandle);

    }
  };

} // end namespace neonGX

#endif // !NEONGX_TEXTUREMANAGER_H
