#include "Font.h"

namespace core {

	FontResource::FontResource(const std::string& path, int32_t fontSize)
		: m_path(path), m_fontSize(fontSize), m_font{} {
	}

	FontResource::~FontResource() {
		if (m_loaded) {
			Unload();
		}
	}

	void FontResource::Load() {
		if (m_loaded) {
			return;
		}

		// Load font with high quality settings
		// Using FONT_DEFAULT and filter for crisp rendering at all resolutions
		m_font = LoadFontEx(m_path.c_str(), m_fontSize, nullptr, 0);

		if (m_font.texture.id == 0) {
			// Failed to load font, use default
			m_font = GetFontDefault();
		}
		else {
			// Set texture filter to bilinear for high quality scaling
			SetTextureFilter(m_font.texture, TEXTURE_FILTER_BILINEAR);
		}

		m_loaded = true;
	}

	void FontResource::Unload() {
		if (!m_loaded) {
			return;
		}

		// Only unload if it's not the default font
		if (m_font.texture.id != GetFontDefault().texture.id) {
			UnloadFont(m_font);
		}

		m_font = {};
		m_loaded = false;
	}

	void FontResource::DrawText(const char* text, float x, float y, float fontSize, Color color) const {
		if (!m_loaded) {
			return;
		}

		DrawTextEx(m_font, text, { x, y }, fontSize, fontSize / 10.0f, color);
	}

	void FontResource::DrawText(const char* text, Vector2 position, float fontSize, float spacing, Color color) const {
		if (!m_loaded) {
			return;
		}

		DrawTextEx(m_font, text, position, fontSize, spacing, color);
	}

	Vector2 FontResource::MeasureText(const char* text, float fontSize, float spacing) const {
		if (!m_loaded) {
			return { 0.0f, 0.0f };
		}

		return MeasureTextEx(m_font, text, fontSize, spacing);
	}

} // namespace core
