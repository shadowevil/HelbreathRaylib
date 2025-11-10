#pragma once
#include "raylib_include.h"
#include "Dependency.h"

namespace core {

	// Font class - wraps raylib Font with dependency management
	class FontResource : public Dependency {
	public:
		FontResource(const std::string& path, int32_t fontSize = 32);
		~FontResource() override;

		void Load() override;
		void Unload() override;

		// Get the raylib font
		const Font& GetFont() const { return m_font; }

		// Get font properties
		const std::string& GetPath() const { return m_path; }
		int32_t GetFontSize() const { return m_fontSize; }

		// Text drawing methods
		void DrawText(const char* text, float x, float y, float fontSize, Color color) const;
		void DrawText(const char* text, Vector2 position, float fontSize, float spacing, Color color) const;
		Vector2 MeasureText(const char* text, float fontSize, float spacing) const;

	private:
		std::string m_path;
		int32_t m_fontSize;
		Font m_font;
	};

	// FontManager - manages a collection of fonts with caching
	template<typename FontEnum>
	class FontManager {
	public:
		FontManager(float cacheTimeout = 30.0f)
			: m_cacheTimeout(cacheTimeout), m_currentTime(0.0f) {}

		~FontManager() {
			UnloadAll();
		}

		// Register a font with an enum key and path
		void RegisterFont(FontEnum key, const std::string& path, int32_t fontSize = 32) {
			uint16_t index = static_cast<uint16_t>(key);
			if (m_fonts.find(index) != m_fonts.end()) {
				// Font already registered, unload the old one
				m_fonts[index]->Unload();
			}
			m_fonts[index] = std::make_unique<FontResource>(path, fontSize);
		}

		// Get a font by enum key (loads if not loaded, updates access time)
		FontResource* GetFont(FontEnum key) {
			uint16_t index = static_cast<uint16_t>(key);
			auto it = m_fonts.find(index);
			if (it == m_fonts.end()) {
				return nullptr;
			}

			auto& font = it->second;
			if (!font->IsLoaded()) {
				font->Load();
			}

			font->UpdateAccessTime(m_currentTime);
			return font.get();
		}

		// Update the cache (call this every frame with delta time)
		void Update(float deltaTime) {
			m_currentTime += deltaTime;

			// Check for fonts that haven't been accessed recently
			if (m_cacheTimeout > 0.0f) {
				for (auto& pair : m_fonts) {
					auto& font = pair.second;
					if (font->IsLoaded()) {
						float timeSinceLastAccess = m_currentTime - font->GetLastAccessTime();
						if (timeSinceLastAccess > m_cacheTimeout) {
							font->Unload();
						}
					}
				}
			}
		}

		// Unload all fonts
		void UnloadAll() {
			for (auto& pair : m_fonts) {
				if (pair.second->IsLoaded()) {
					pair.second->Unload();
				}
			}
		}

		// Force load all fonts (useful for preloading)
		void LoadAll() {
			for (auto& pair : m_fonts) {
				if (!pair.second->IsLoaded()) {
					pair.second->Load();
				}
			}
		}

		// Set cache timeout (0 = no auto-unload)
		void SetCacheTimeout(float timeout) { m_cacheTimeout = timeout; }

		// Get cache timeout
		float GetCacheTimeout() const { return m_cacheTimeout; }

	private:
		std::unordered_map<uint16_t, std::unique_ptr<FontResource>> m_fonts;
		float m_cacheTimeout;
		float m_currentTime;
	};

} // namespace core
