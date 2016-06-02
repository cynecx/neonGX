/*
 * neonGX - Container.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Display/Container.hpp>
#include <limits>

namespace neonGX {

  Container::Container() = default;
  Container::~Container() = default;

  void Container::AddChild(std::shared_ptr<DisplayObject> child) {
    assert(child.get() != nullptr);
    m_Children.push_back(child);
    DisplayObject::SetRemoteDOParent(child.get());
  }

  void Container::AddChildAt(std::shared_ptr<DisplayObject> child,
                             size_t index) {
    assert(child.get() != nullptr);
    assert(index < m_Children.size());
    m_Children.insert(m_Children.begin() + int(index), child);
    DisplayObject::SetRemoteDOParent(child.get());
  }

  void Container::RemoveChild(size_t index) {
    assert(index < m_Children.size());
    auto it = m_Children.begin() + size_t(index);
    DisplayObject::SetRemoteDOParent(it->get(), true);
    m_Children.erase(it);
  }

  std::shared_ptr<DisplayObject>& Container::GetChildAt(size_t index) {
    assert(index < m_Children.size());
    return m_Children[index];
  }

  void Container::UpdateTransform(bool UseDefaultParent) {
    if(!m_Visible) {
      return;
    }

    DisplayObject::UpdateTransform(UseDefaultParent);

    for(auto& child : m_Children) {
      child->UpdateTransform();
    }
  }

  NRectangle Container::GetBounds() {
    if(m_Children.size() == 0) {
      return { {0, 0}, {0, 0} };
    }

    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();

    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();

    bool hasVisibleChild = false;

    for(auto& child : m_Children) {
      if(!child->GetVisible()) {
        continue;
      }

      auto childBounds = child->GetBounds();
      if(childBounds.IsNull()) {
        continue;
      }

      hasVisibleChild = true;

      minX = (minX < childBounds.point.x) ? minX : childBounds.point.x;
      minY = (minY < childBounds.point.y) ? minY : childBounds.point.y;

      int childMaxX = childBounds.size.width + childBounds.point.x;
      int childMaxY = childBounds.size.height + childBounds.point.y;

      maxX = (maxX > childMaxX) ? maxX : childMaxX;
      maxY = (maxY > childMaxY) ? maxY : childMaxY;
    }

    if(!hasVisibleChild) {
      return { {0, 0}, {0, 0} };
    }

    return { {minX, minY}, { maxX - minX, maxY - minY } };
  }

  NRectangle Container::GetLocalBounds() {
    for(auto& it : m_Children) {
      it->UpdateTransform(true);
    }

    return GetBounds();
  }

  void Container::RenderWebGL(GLRenderer* renderer) {
    assert(renderer != nullptr);

    if(!m_Visible || m_Alpha <= 0 || !m_Renderable) {
      return;
    }

    for(auto& it : m_Children) {
      it->RenderWebGL(renderer);
    }
  }
}
