/*
 * neonGX - Texture.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TEXTURE_H
#define NEONGX_TEXTURE_H

#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Textures/BaseTexture.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>
#include <memory>
#include <neonGX/Core/ADT.hpp>
#include <unordered_map>

namespace neonGX {

  struct TextureUVs {
    FPoint P0{0.0f, 0.0f};
    FPoint P1{1.0f, 0.0f};
    FPoint P2{1.0f, 1.0f};
    FPoint P3{0.0f, 1.0f};

    TextureUVs() = default;

    void Set(const FRectangle& frame, const FSize& baseFrame);
  };

  class Texture {
  private:
    bool m_IsValid = false;

    std::shared_ptr<BaseTexture> m_BaseTexture;
    optional<TextureUVs> m_UVs;

    FRectangle m_Frame;
    FRectangle m_Orig{ {0, 0}, {1, 1} };

  public:
    Texture(std::shared_ptr<BaseTexture> baseTexture,
            optional<FRectangle> frame);

    ~Texture() = default;

    Texture(const Texture&) = default;
    Texture& operator=(const Texture&) = default;

    TextureUVs& GetUVs() {
      assert(m_IsValid && m_UVs);
      return m_UVs.value();
    }

    bool IsValid() {
      return m_IsValid && m_UVs;
    }

    std::shared_ptr<BaseTexture> GetBaseTexture() {
      return m_BaseTexture;
    }

    FRectangle GetFrame() const {
      return m_Frame;
    }

    void SetFrame(const FRectangle& frame);

    FSize GetSize() const {
      return {
          m_Orig.size.width,
          m_Orig.size.height
      };
    }
  };

} // end namespace neonGX

#endif // !NEONGX_TEXTURE_H
