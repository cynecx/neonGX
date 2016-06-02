/*
 * neonGX - neonGX.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_NEONGX_H
#define NEONGX_NEONGX_H

#include <functional>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>

namespace neonGX {

  void InitializeEngine();

  void RunMainLoop(webgl_context_handle glHandle,
                   std::function<void()> loopCallback);

} // end namespace neonGX

#endif // !NEONGX_NEONGX_H
