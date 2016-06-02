/*
 * neonGX - ObjectRenderer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_OBJECTRENDERER_H
#define NEONGX_OBJECTRENDERER_H

#include <neonGX/Core/Display/DisplayObject.hpp>

namespace neonGX {

  struct ObjectRenderer {
    ObjectRenderer();
    virtual ~ObjectRenderer();

    virtual void Start() = 0;

    virtual void Stop() {
      Flush();
    }

    virtual void Flush() = 0;

    virtual void Render(DisplayObject* object) = 0;
  };

} // end namespace neonGX

#endif // !NEONGX_OBJECTRENDERER_H
