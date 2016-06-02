/*
 * neonGX - GLContext.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>

namespace neonGX {

#ifndef NEONGX_USE_EMSCRIPTEN
  thread_local webgl_context_handle WebGLContextRAII::threadGlobContextHandle =
      invalid_context_handle;
#endif

}
