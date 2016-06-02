/*
 * neonGX - DisplayObject.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_DISPLAYOBJECT_H
#define NEONGX_DISPLAYOBJECT_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Math/Matrix.hpp>
#include <neonGX/Core/Math/Transformation.hpp>
#include <neonGX/Core/Renderer/Renderer.hpp>

namespace neonGX {

  class Container;
  class GLRenderer;

  class DisplayObject {
  public:
    DisplayObject();
    virtual ~DisplayObject();

  protected:
    FPoint m_Position{};
    FPoint m_Scale{1.0f, 1.0f};
    NPoint m_Pivot{};
    NPoint m_Skew{};

    float m_Rotation = 0;
    float m_Alpha = 1;

    bool m_Visible = true;
    bool m_Renderable = true;

    Matrix3 m_LocalTransform;
    Matrix3 m_WorldTransform = GetIdentityTransform();

    float m_WorldAlpha = 1;

    DisplayObject* m_Parent = nullptr;

    NRectangle bounds{{0, 0}, {1, 1}};

    void SetRemoteDOParent(DisplayObject* displayObject, bool null = false) {
      displayObject->m_Parent = null ? nullptr : this;
    }

  public:
    FPoint GetPosition() const {
      return m_Position;
    }

    void SetPosition(const FPoint& position) {
      if(position == m_Position) {
        return;
      }
      OnStateChanged();
      m_Position = position;
    }

    FPoint GetScale() const {
      return m_Scale;
    }

    void SetScale(const FPoint& scale) {
      if(scale == m_Scale) {
        return;
      }
      OnStateChanged();
      m_Scale = scale;
    }

    NPoint GetPivot() const {
      return m_Pivot;
    }

    void SetPivot(const NPoint& pivot) {
      if(pivot == m_Pivot) {
        return;
      }
      OnStateChanged();
      m_Pivot = pivot;
    }

    NPoint GetSkew() const {
      return m_Skew;
    }

    void SetSkew(const NPoint& skew) {
      if(skew == m_Skew) {
        return;
      }
      OnStateChanged();
      m_Skew = skew;
    }

    float GetRotation() const {
      return m_Rotation;
    }

    void SetRotation(float rotation) {
      if(rotation == m_Rotation) {
        return;
      }
      OnStateChanged();
      m_Rotation = rotation;
    }

    float GetAlpha() const {
      return m_Alpha;
    }

    void SetAlpha(float alpha) {
      if(alpha == m_Alpha) {
        return;
      }
      OnStateChanged();
      m_Alpha = alpha;
    }

    bool IsVisible() const;

    bool GetVisible() const {
      return m_Visible;
    }

    void SetVisible(bool visible) {
      if(visible == m_Visible) {
        return;
      }
      OnStateChanged();
      m_Visible = visible;
    }

    FPoint ToGlobalPosition(const FPoint& position);
    FPoint ToLocalPosition(const FPoint& position, DisplayObject* fromObj);

    virtual NRectangle GetBounds() {
      return { {0, 0}, {0, 0} };
    }

    virtual NRectangle GetLocalBounds() {
      return GetBounds();
    }

    virtual void OnStateChanged() = 0;

    virtual void RenderWebGL(GLRenderer* renderer) = 0;

    virtual void UpdateTransform(bool UseIdentityTransform = false);
  };

} // end namespace neonGX

#endif // !NEONGX_DISPLAYOBJECT_H
