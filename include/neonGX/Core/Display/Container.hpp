/*
 * neonGX - Container.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_CONTAINER_H
#define NEONGX_CONTAINER_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Display/DisplayObject.hpp>
#include <memory>
#include <vector>

namespace neonGX {

  class Container : public DisplayObject {
  private:
    std::vector<std::shared_ptr<DisplayObject>> m_Children;

  public:
    Container();
    virtual ~Container();

    void AddChild(std::shared_ptr<DisplayObject> child);
    void AddChildAt(std::shared_ptr<DisplayObject> child, size_t index);
    void RemoveChild(size_t index);
    std::shared_ptr<DisplayObject>& GetChildAt(size_t index);

    void UpdateTransform(bool UseDefaultParent = false) override;

    NRectangle GetBounds() override;
    NRectangle GetLocalBounds() override;

    virtual void OnStateChanged() override {
    }

    void RenderWebGL(GLRenderer* renderer) override;
  };

} // end namespace neonGX

#endif // !NEONGX_CONTAINER_H
