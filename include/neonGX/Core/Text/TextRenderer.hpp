/*
 * neonGX - TextRenderer.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_TEXTRENDERER_H
#define NEONGX_TEXTRENDERER_H

#include <neonGX/Core/Renderer/OpenGL/ObjectRenderer.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLBuffer.hpp>
#include <string>
#include <vector>

namespace neonGX {

  class Text;

  class TextRenderer final : public ObjectRenderer {
  private:
    static constexpr size_t VertexDataCount = 7;
    static constexpr size_t VertexByteSize = VertexDataCount * sizeof(float);
    static constexpr size_t BatchSize = 2000;

    webgl_context_handle m_GLHandle;
    GLRenderer* m_Renderer;

    std::vector<uint8_t> m_Vertices;
    std::vector<uint16_t> m_Indices;

    std::shared_ptr<GLBuffer> m_VertexBuffer;
    std::shared_ptr<GLBuffer> m_IndexBuffer;

    std::vector<Text*> m_TextObjects;

    size_t m_CurrentBatchCount = 0;

  public:
    explicit TextRenderer(GLRenderer* renderer);
    virtual ~TextRenderer();

    void Start() override;
    void Stop() override;
    void Flush() override;
    void Render(DisplayObject* object) override;
  };

} // end namespace neonGX

#endif // !NEONGX_TEXTRENDERER_H
