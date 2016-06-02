/*
 * neonGX - SpriteRenderer.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Sprites/SpriteRenderer.hpp>
#include <neonGX/Core/Sprites/Sprite.hpp>

namespace neonGX {

  SpriteRenderer::SpriteRenderer(GLRenderer* renderer) : m_Renderer(renderer) {
    m_GLHandle = renderer->webgl_handle;

    WebGLContextRAII switchCtx(m_GLHandle);

    m_Vertices.resize(BatchSize * 4 * VertexByteSize);

    m_Renderer->m_ShaderManager->InitializeTextureShader();

    CreateIndicesForQuads();

    gsl::span<const uint8_t> indices(
        reinterpret_cast<const uint8_t*>(m_Indices.data()),
        m_Indices.size() * sizeof(uint16_t));

    gsl::span<const uint8_t> vertices(
        reinterpret_cast<const uint8_t*>(m_Vertices.data()),
        m_Vertices.size() * sizeof(uint8_t));

    m_IndexBuffer = std::shared_ptr<GLBuffer>(
        GLBuffer::CreateIndexBuffer(m_GLHandle, indices, GL_STATIC_DRAW));

    m_VertexBuffer = std::shared_ptr<GLBuffer>(
        GLBuffer::CreateVertexBuffer(m_GLHandle, vertices, GL_DYNAMIC_DRAW));
  }

  SpriteRenderer::~SpriteRenderer() = default;

  void SpriteRenderer::CreateIndicesForQuads() {
    size_t totalIndices = BatchSize * 6;
    m_Indices.resize(totalIndices);

    uint16_t j = 0;
    for(size_t i = 0; i < totalIndices; i += 6, j += 4) {
      m_Indices[i + 0] = uint16_t(j + 0);
      m_Indices[i + 1] = uint16_t(j + 1);
      m_Indices[i + 2] = uint16_t(j + 2);
      m_Indices[i + 3] = uint16_t(j + 0);
      m_Indices[i + 4] = uint16_t(j + 2);
      m_Indices[i + 5] = uint16_t(j + 3);
    }
  }

  void SpriteRenderer::Start() {
    WebGLContextRAII switchCtx(m_GLHandle);

    auto& textureShader = m_Renderer->m_ShaderManager->UseTextureShader();
    m_VertexBuffer->Bind();
    m_IndexBuffer->Bind();

    m_Renderer->m_ShaderManager->ActivateShaderAttributes();

    glVertexAttribPointer(
        textureShader.m_shaderAttributes["aVertexPosition"].Location,
        2, GL_FLOAT, GL_FALSE, VertexByteSize,
        reinterpret_cast<const void*>(0));
    glVertexAttribPointer(
        textureShader.m_shaderAttributes["aTextureCoord"].Location,
        2, GL_FLOAT, GL_FALSE, VertexByteSize,
        reinterpret_cast<const void*>(2 * sizeof(float)));
    glVertexAttribPointer(
        textureShader.m_shaderAttributes["aColor"].Location,
        4, GL_UNSIGNED_BYTE, GL_TRUE, VertexByteSize,
        reinterpret_cast<const void*>(4 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
  }

  void SpriteRenderer::Stop() {
    Flush();
  }

  void SpriteRenderer::Flush() {
    if(m_Sprites.size() == 0) {
      return;
    }

    WebGLContextRAII switchCtx(m_GLHandle);

    float* floatView = reinterpret_cast<float*>(m_Vertices.data());
    uint32_t* uint32View = reinterpret_cast<uint32_t*>(m_Vertices.data());

    size_t bufferIndex = 0;

    for(auto sprite : m_Sprites) {
      uint32_t tint = (sprite->m_Tint >> 16) +
          (sprite->m_Tint & 0xFF00) +
          ((sprite->m_Tint & 0xFF) << 16) +
          (uint32_t(sprite->m_WorldAlpha * 255) << 24);

      TextureUVs& uvs = sprite->m_Texture->GetUVs();

      floatView[bufferIndex++] = sprite->m_VertexData[0];
      floatView[bufferIndex++] = sprite->m_VertexData[1];
      floatView[bufferIndex++] = uvs.P0.x;
      floatView[bufferIndex++] = uvs.P0.y;
      uint32View[bufferIndex++] = tint;

      floatView[bufferIndex++] = sprite->m_VertexData[2];
      floatView[bufferIndex++] = sprite->m_VertexData[3];
      floatView[bufferIndex++] = uvs.P1.x;
      floatView[bufferIndex++] = uvs.P1.y;
      uint32View[bufferIndex++] = tint;

      floatView[bufferIndex++] = sprite->m_VertexData[4];
      floatView[bufferIndex++] = sprite->m_VertexData[5];
      floatView[bufferIndex++] = uvs.P2.x;
      floatView[bufferIndex++] = uvs.P2.y;
      uint32View[bufferIndex++] = tint;

      floatView[bufferIndex++] = sprite->m_VertexData[6];
      floatView[bufferIndex++] = sprite->m_VertexData[7];
      floatView[bufferIndex++] = uvs.P3.x;
      floatView[bufferIndex++] = uvs.P3.y;
      uint32View[bufferIndex++] = tint;
    }

    size_t batchDataSize = m_Sprites.size() * VertexByteSize * 4;
    gsl::span<const uint8_t> tmpSpan(m_Vertices.data(), batchDataSize);

    m_VertexBuffer->UploadSubData(tmpSpan);

    auto renderBatch = [] (webgl_context_handle glHandle,
                           const std::shared_ptr<BaseTexture>& texture,
                           size_t size, size_t startIndex) {
      if(size == 0) {
        return;
      }

      assert(texture.get() != nullptr);
      texture->GetGLTexture(glHandle)->Bind(nullopt);

      glDrawElements(GL_TRIANGLES, GLsizei(size * 6), GL_UNSIGNED_SHORT,
          reinterpret_cast<const void*>(startIndex * 6 * 2));
    };

#ifdef NEONGX_USE_EMSCRIPTEN
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#else
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

    auto& textureShader = m_Renderer->m_ShaderManager->UseTextureShader();
    textureShader.SetProjectionMatrix(
        m_Renderer->m_RenderTarget->m_ProjectionMatrix);

    size_t batchSize = 0;
    size_t startBatch = 0;

    std::shared_ptr<BaseTexture> currentBaseTexture;
    std::shared_ptr<BaseTexture> nextBaseTexture;

    for(size_t i = 0; i < m_Sprites.size(); i++) {
      auto sprite = m_Sprites[i];

      nextBaseTexture = sprite->m_Texture->GetBaseTexture();

      if(currentBaseTexture != nextBaseTexture) {
        renderBatch(m_GLHandle, currentBaseTexture, batchSize, startBatch);

        startBatch = i;
        batchSize = 0;
        currentBaseTexture = nextBaseTexture;
      }

      batchSize++;
    }

    renderBatch(m_GLHandle, currentBaseTexture, batchSize, startBatch);

    m_Sprites.clear();
  }

  void SpriteRenderer::Render(DisplayObject *object) {
    assert(m_Sprites.size() <= BatchSize);

    if(m_Sprites.size() == BatchSize) {
      Flush();
    }

    auto* sprite = dynamic_cast<Sprite*>(object);
    assert(sprite != nullptr);
    assert(sprite->m_Texture->IsValid());

    m_Sprites.push_back(sprite);
  }

}
