/*
 * neonGX - TextRenderer.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Text/TextRenderer.hpp>
#include <neonGX/Core/Text/Text.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLRenderer.hpp>
#include <algorithm>

namespace neonGX {

  static void CreateIndicesForQuads(size_t batchSize,
                                    std::vector<uint16_t>& indices) {
    size_t totalIndices = batchSize * 6;
    indices.resize(totalIndices);

    uint16_t j = 0;
    for(size_t i = 0; i < totalIndices; i += 6, j += 4) {
      indices[i + 0] = uint16_t(j + 0);
      indices[i + 1] = uint16_t(j + 1);
      indices[i + 2] = uint16_t(j + 2);
      indices[i + 3] = uint16_t(j + 0);
      indices[i + 4] = uint16_t(j + 2);
      indices[i + 5] = uint16_t(j + 3);
    }
  }

  TextRenderer::TextRenderer(GLRenderer* renderer) : m_Renderer(renderer) {
    m_GLHandle = renderer->webgl_handle;

    WebGLContextRAII switchCtx(m_GLHandle);

    m_Vertices.resize(BatchSize * 4 * VertexByteSize);
    CreateIndicesForQuads(BatchSize, m_Indices);

    m_Renderer->m_ShaderManager->InitializeFontTextureShader();

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

  TextRenderer::~TextRenderer() = default;

  void TextRenderer::Start() {
    WebGLContextRAII switchCtx(m_GLHandle);

    auto& textureShader = m_Renderer->m_ShaderManager->UseFontTextureShader();
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
        3, GL_FLOAT, GL_FALSE, VertexByteSize,
        reinterpret_cast<const void*>(4 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
  }

  void TextRenderer::Stop() {
    Flush();
  }

  void TextRenderer::Flush() {
    m_CurrentBatchCount = 0;

    if(m_TextObjects.empty()) {
      return;
    }

    WebGLContextRAII switchCtx(m_GLHandle);

    float* floatView = reinterpret_cast<float*>(m_Vertices.data());

    size_t bufferIndex = 0;
    size_t vertexIndex = 0;
    size_t sumTextureToRender = 0;

    for(auto textObj : m_TextObjects) {
      float red = float(textObj->m_Color.r) / 255;
      float green = float(textObj->m_Color.g) / 255;
      float blue = float(textObj->m_Color.b) / 255;

      for(size_t i = 0; i < textObj->m_Text.size(); i++) {
        if(textObj->m_ExcludeSet.find(i) != textObj->m_ExcludeSet.end()) {
          vertexIndex += 8;
          continue;
        }

        sumTextureToRender++;

        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = 0;
        floatView[bufferIndex++] = 0;
        floatView[bufferIndex++] = red;
        floatView[bufferIndex++] = green;
        floatView[bufferIndex++] = blue;

        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = 1;
        floatView[bufferIndex++] = 0;
        floatView[bufferIndex++] = red;
        floatView[bufferIndex++] = green;
        floatView[bufferIndex++] = blue;

        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = 1;
        floatView[bufferIndex++] = 1;
        floatView[bufferIndex++] = red;
        floatView[bufferIndex++] = green;
        floatView[bufferIndex++] = blue;

        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = textObj->m_VertexData[vertexIndex++];
        floatView[bufferIndex++] = 0;
        floatView[bufferIndex++] = 1;
        floatView[bufferIndex++] = red;
        floatView[bufferIndex++] = green;
        floatView[bufferIndex++] = blue;
      }
    }

    size_t batchDataSize = sumTextureToRender * VertexByteSize * 4;
    gsl::span<const uint8_t> tmpSpan(m_Vertices.data(), batchDataSize);

    m_VertexBuffer->UploadSubData(tmpSpan);

    auto renderBatch = [] (webgl_context_handle glHandle,
                           const GLTexture* texture,
                           size_t size, size_t startIndex) {
      if(size == 0 || texture == nullptr) {
        return;
      }
      texture->Bind(nullopt);
      glDrawElements(GL_TRIANGLES, GLsizei(size * 6), GL_UNSIGNED_SHORT,
          reinterpret_cast<const void*>(startIndex * 6 * 2));
    };

#ifdef NEONGX_USE_EMSCRIPTEN
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#else
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif

    auto& shader = m_Renderer->m_ShaderManager->UseFontTextureShader();
    shader.SetProjectionMatrix(
        m_Renderer->m_RenderTarget->m_ProjectionMatrix);

    size_t batchSize = 0;
    size_t startBatch = 0;

    const GLTexture* currentBaseTexture = nullptr;
    const GLTexture* nextBaseTexture = nullptr;

    size_t index = 0;

    for(auto to : m_TextObjects) {
      auto optFontData = m_Renderer->m_FontTextureManager->GetFontData(
          to->m_FontName);
      assert(optFontData);

      const FontData* fontData = optFontData.value();

      for(size_t i = 0; i < to->m_Text.size(); i++) {
        if(to->m_ExcludeSet.find(i) != to->m_ExcludeSet.end()) {
          continue;
        }

        auto it = fontData->CharTextures.find(char16_t(to->m_Text[i]));
        const auto& glyphInfo = it->second;

        nextBaseTexture = &glyphInfo.Texture;

        if(currentBaseTexture != nextBaseTexture) {
          renderBatch(m_GLHandle, currentBaseTexture, batchSize, startBatch);

          startBatch = index;
          batchSize = 0;
          currentBaseTexture = nextBaseTexture;
        }

        ++index;
        ++batchSize;
      }
    }

    renderBatch(m_GLHandle, currentBaseTexture, batchSize, startBatch);

    m_TextObjects.clear();
  }

  void TextRenderer::Render(DisplayObject* object) {
    assert(m_CurrentBatchCount <= BatchSize);

    if(m_CurrentBatchCount == BatchSize) {
      Flush();
    }

    auto* textObj = dynamic_cast<Text*>(object);
    assert(textObj != nullptr);

    size_t addTextures = textObj->GetText().size();
    if(m_CurrentBatchCount + addTextures > BatchSize) {
      Flush();
    }

    // Every character is a single texture
    m_CurrentBatchCount += textObj->GetText().size();
    m_TextObjects.push_back(textObj);
  }

}
