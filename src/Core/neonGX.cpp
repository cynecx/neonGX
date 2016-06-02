/*
 * neonGX - neonGX.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/neonGX.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>

#include <cassert>

#ifdef NEONGX_USE_EMSCRIPTEN
#include <emscripten.h>
#endif

namespace neonGX {

  namespace {
    bool neonGXInitialized = false;
    std::function<void()> neonGXMainLoop;
  }

  void InitializeEngine() {
#ifndef NEONGX_USE_EMSCRIPTEN
    glfwInit();
    glfwSwapInterval(1);
#endif
    neonGXInitialized = true;
  }

  static void ProxyMainLoop() {
    assert(neonGXMainLoop);
    neonGXMainLoop();
  }

  void RunMainLoop(webgl_context_handle glHandle,
                   std::function<void()> loopCallback) {
    assert(neonGXInitialized);
    assert(!neonGXMainLoop);

    neonGXMainLoop = loopCallback;
#ifdef NEONGX_USE_EMSCRIPTEN
    emscripten_set_main_loop(ProxyMainLoop, 0, false);
    emscripten_exit_with_live_runtime();
#endif

#ifndef NEONGX_USE_EMSCRIPTEN
    while (true) {
      glfwPollEvents();

      if(glfwWindowShouldClose(glHandle)) {
        break;
      }

      ProxyMainLoop();
      glfwSwapBuffers(glHandle);
    }
#endif
  }

}
