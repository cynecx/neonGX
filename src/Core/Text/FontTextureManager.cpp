/*
 * neonGX - FontManager.cpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#include <neonGX/Core/Text/FontTextureManager.hpp>
#include <neonGX/Core/Math/Primitives.hpp>
#include <iostream>
#include <exception>

namespace neonGX {

  namespace {
    struct FTFontFaceRAII {
      FT_Face fontFace = nullptr;

      explicit FTFontFaceRAII(FT_Face _faceFont) : fontFace(_faceFont) {}

      ~FTFontFaceRAII() {
        if(fontFace) {
          return;
        }

        FT_Done_Face(fontFace);
      }
    };

    struct PixelStoreiRAII {
      GLenum Mode;
      GLint OldValue = 0;

      explicit PixelStoreiRAII(GLenum mode, GLint newVal) : Mode(mode) {
        glGetIntegerv(mode, &OldValue);
        assert(OldValue != GL_INVALID_ENUM);
        if(OldValue == newVal) {
          OldValue = GL_INVALID_ENUM;
          return;
        }
        glPixelStorei(Mode, newVal);
      }

      ~PixelStoreiRAII() {
        if(OldValue == GL_INVALID_ENUM) {
          return;
        }
        glPixelStorei(Mode, OldValue);
      }
    };
  }

  FontTextureManager::FontTextureManager(webgl_context_handle glHandle)
      : m_GlHandle(glHandle) {
    int result = FT_Init_FreeType(&m_ft2Instance);
    assert(result == 0);
    ((void)result);
  }

  FontTextureManager::~FontTextureManager() = default;

  bool FontTextureManager::LoadFont(const std::string& filename,
                                    const std::string& fontName,
                                    size_t height) {
    if(IsValidKey(fontName)) {
      return false;
    }

    WebGLContextRAII switchCtx(m_GlHandle);
    PixelStoreiRAII pixelUnpack(GL_UNPACK_ALIGNMENT, 1);

    FT_Face fontface = nullptr;
    if(FT_New_Face(m_ft2Instance, filename.c_str(), 0, &fontface)) {
      return false;
    }

    FTFontFaceRAII fontfaceRAII(fontface);
    FT_Set_Pixel_Sizes(fontface, 0, height);

    FontData fontData;
    fontData.FontSource = filename;

    int32_t MaxDescent = 0;

    for(uint8_t currChar = 0; currChar < 128; currChar++) {
      if(FT_Load_Char(fontface, currChar, FT_LOAD_RENDER)) {
        continue;
      }

      MaxDescent = std::max<int32_t>(fontface->glyph->bitmap_top, MaxDescent);

      GLTexture texture(m_GlHandle, nullopt, GL_LUMINANCE, GL_UNSIGNED_BYTE);

      texture.EnableWrapClamp();
      texture.SetMagFilter(ScaleMode::Linear);
      texture.SetMinFilter(ScaleMode::Linear);

      bool hasSize = fontface->glyph->bitmap.width > 0 ||
                     fontface->glyph->bitmap.rows > 0;
      if(hasSize) {
        int dataLen = fontface->glyph->bitmap.width *
                      fontface->glyph->bitmap.rows;
        texture.UploadData({fontface->glyph->bitmap.buffer, dataLen},
            FSize{float(fontface->glyph->bitmap.width),
                  float(fontface->glyph->bitmap.rows)});
      }

      FontCharacterData character{
          {std::move(texture)},
          fontface->glyph->bitmap_left,
          fontface->glyph->bitmap_top,
          int32_t(fontface->glyph->advance.x),
          0
      };

      fontData.CharTextures.emplace(char16_t(currChar), std::move(character));
    }

    for(auto& it : fontData.CharTextures) {
      it.second.DescentY = MaxDescent;
    }

    m_LoadedFontMap.emplace(fontName, std::move(fontData));

    return true;
  }

  optional<const FontData*> FontTextureManager::GetFontData(
      const std::string& fontName) const {
    auto it = m_LoadedFontMap.find(fontName);
    if(it == m_LoadedFontMap.end()) {
      return nullopt;
    }
    return {&it->second};
  }

  void FontTextureManager::DeleteFontData(const std::string& fontName) {
    m_LoadedFontMap.erase(fontName);
  }

  void FontTextureManager::Clear() {
    m_LoadedFontMap.clear();
  }

}
