/*
 * neonGX - GLContext.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_GLCONTEXT_H
#define NEONGX_GLCONTEXT_H

#include <cstdint>

#ifdef NEONGX_USE_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#else
#include <GLFW/glfw3.h>
#include <GL/glcorearb.h>
#endif

namespace neonGX {

#ifdef NEONGX_USE_EMSCRIPTEN
  using webgl_context_handle = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;
  static constexpr webgl_context_handle invalid_context_handle = -1;

  inline webgl_context_handle neongx_get_current_gl_context() {
    return emscripten_webgl_get_current_context();
  }

  inline void neongx_set_gl_context(webgl_context_handle context) {
    emscripten_webgl_make_context_current(context);
  }

  enum : GLenum {
    GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL = 37441
  };

  class WebGLContextRAII {
  private:
    webgl_context_handle m_SavedHandle;
    bool m_Restored = false;

  public:
    WebGLContextRAII(webgl_context_handle handle) {
      m_SavedHandle = neongx_get_current_gl_context();
      if(m_SavedHandle != handle) {
        neongx_set_gl_context(handle);
      } else {
        m_Restored = true;
      }
    }

    ~WebGLContextRAII() {
      if(!m_Restored)
        neongx_set_gl_context(m_SavedHandle);
    }

    void Restore() {
      if(m_Restored) {
        return;
      }
      neongx_set_gl_context(m_SavedHandle);
      m_Restored = true;
    }
  };
#else
  using webgl_context_handle = GLFWwindow*;
  static constexpr webgl_context_handle invalid_context_handle = nullptr;

  inline webgl_context_handle neongx_get_current_gl_context() {
    return glfwGetCurrentContext();
  }

  inline void neongx_set_gl_context(webgl_context_handle context) {
    glfwMakeContextCurrent(context);
  }

  class WebGLContextRAII {
  private:
    static thread_local webgl_context_handle threadGlobContextHandle;

    webgl_context_handle m_SavedHandle = invalid_context_handle;
    bool m_Restored = false;

  public:
    WebGLContextRAII(webgl_context_handle handle) {
      if(threadGlobContextHandle == invalid_context_handle) {
        threadGlobContextHandle = neongx_get_current_gl_context();
      }

      if(threadGlobContextHandle == handle) {
        m_Restored = true;
        return;
      }

      m_SavedHandle = threadGlobContextHandle;
      threadGlobContextHandle = handle;

      neongx_set_gl_context(handle);
    }

    ~WebGLContextRAII() {
      Restore();
    }

    void Restore() {
      if(m_Restored) {
        return;
      }

      threadGlobContextHandle = m_SavedHandle;
      neongx_set_gl_context(m_SavedHandle);

      m_Restored = true;
    }
  };
#endif

} // end namespace neonGX

#endif // !NEONGX_GLCONTEXT_H
