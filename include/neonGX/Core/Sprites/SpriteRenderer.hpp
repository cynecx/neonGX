/*
 * neonGX - SpriteRenderer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_SPRITERENDERER_H
#define NEONGX_SPRITERENDERER_H

#include <neonGX/Core/Math/Helpers.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/ObjectRenderer.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLBuffer.hpp>
#include <neonGX/Core/Renderer/OpenGL/Shaders/TextureShader.hpp>
#include <neonGX/Core/Textures/Texture.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace neonGX {

  class Sprite;

  class SpriteRenderer final : public ObjectRenderer {
  private:
    // position{X, Y} = 2 x FPoint, Color{R, G, B} = 4 x byte (Normalized)
    static constexpr size_t VertexDataCount = 5;
    static constexpr size_t VertexByteSize = VertexDataCount * sizeof(float);
    static constexpr size_t BatchSize = 2000;

    webgl_context_handle m_GLHandle;

    std::vector<uint8_t> m_Vertices;
    std::vector<uint16_t> m_Indices;

    std::shared_ptr<GLBuffer> m_VertexBuffer;
    std::shared_ptr<GLBuffer> m_IndexBuffer;

    std::vector<Sprite*> m_Sprites;

    GLRenderer* m_Renderer;

  public:
    explicit SpriteRenderer(GLRenderer* renderer);
    virtual ~SpriteRenderer();

  private:
    void CreateIndicesForQuads();

  public:
    void Start() override;
    void Stop() override;
    void Flush() override;
    void Render(DisplayObject* object) override;
  };

} // end namespace neonGX

#endif // !NEONGX_SPRITERENDERER_H
