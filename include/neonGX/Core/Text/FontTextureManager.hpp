/*
 * neonGX - FontTextureManager.hpp
 *
 * Copyright (c) 2016, cynecx
 *
 * This file is part of neonGX and is distributed under a modified
 * BSD 3-Clause License. See LICENSE.txt for details.
 *
 */

#ifndef NEONGX_FONTTEXTUREMANAGER_H
#define NEONGX_FONTTEXTUREMANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <neonGX/Core/ADT.hpp>
#include <neonGX/Core/Math/Primitives.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLContext.hpp>
#include <neonGX/Core/Renderer/OpenGL/GLTexture.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace neonGX {

  struct FontCharacterData {
    GLTexture Texture;
    int32_t BearingX;
    int32_t BearingY;
    int32_t AdvanceX;
    int32_t DescentY;
  };

  struct FontData {
    std::string FontSource;
    std::unordered_map<char16_t, FontCharacterData> CharTextures;
  };

  class FontTextureManager {
  private:
    FT_Library m_ft2Instance = nullptr;
    webgl_context_handle m_GlHandle;

    std::unordered_map<std::string, FontData> m_LoadedFontMap;

  public:
    explicit FontTextureManager(webgl_context_handle glHandle);
    ~FontTextureManager();

    FontTextureManager(const FontTextureManager&) = delete;
    FontTextureManager& operator=(const FontTextureManager&) = delete;

    bool LoadFont(const std::string& filename, const std::string& fontName,
                  size_t height);

    optional<const FontData*> GetFontData(const std::string& key) const;

    bool IsValidKey(const std::string& key) const {
      return m_LoadedFontMap.find(key) != m_LoadedFontMap.end();
    }

    void DeleteFontData(const std::string& fontName);

    void Clear();
  };

} // end namespace neonGX

#endif // !NEONGX_FONTTEXTUREMANAGER_H
