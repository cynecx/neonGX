/*
 * neonGX - DisplayObject.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Display/DisplayObject.hpp>
#include <neonGX/Core/Display/Container.hpp>

namespace neonGX {

  DisplayObject::DisplayObject() = default;
  DisplayObject::~DisplayObject() = default;

  void DisplayObject::UpdateTransform(bool UseIdentityTransform) {
    m_LocalTransform = GetSkewingMatrix(m_Skew) *
                       GetTranslationMatrix(m_Position) *
                       GetTranslationMatrix(m_Pivot) *
                       GetRotationMatrix(m_Rotation) *
                       GetScalingMatrix(m_Scale) *
                       GetTranslationMatrix(m_Pivot.Neg());

    if(m_Parent && !UseIdentityTransform) {
      m_WorldTransform = m_LocalTransform * m_Parent->m_LocalTransform;
      m_WorldAlpha = m_Alpha * m_Parent->m_Alpha;
    } else {
      m_WorldTransform = m_LocalTransform * GetIdentityTransform();
      m_WorldAlpha = m_Alpha;
    }
  }

  bool DisplayObject::IsVisible() const {
    if(!m_Parent) {
      return m_Visible;
    }

    for(DisplayObject* obj = m_Parent; obj != nullptr; obj = obj->m_Parent) {
      if(!obj->m_Visible) {
        return false;
      }
    }

    return true;
  }

  FPoint DisplayObject::ToGlobalPosition(const FPoint& position) {
    UpdateTransform();
    return ApplyTransformation(m_WorldTransform, position);
  }

  FPoint DisplayObject::ToLocalPosition(const FPoint& position,
                                        DisplayObject* fromObj) {
    FPoint newPos;
    if(fromObj) {
      newPos = fromObj->ToGlobalPosition(position);
    } else {
      newPos = position;
    }

    UpdateTransform();
    return ApplyInverseTransformation(m_WorldTransform, newPos);
  }

}
