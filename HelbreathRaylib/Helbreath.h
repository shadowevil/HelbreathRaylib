#pragma once
#include "raylib_include.h"
#include "core/Application.h"
#include "core/Font.h"

// Font types for the Helbreath application
enum class FontType : uint16_t {
	Default = 0,
	Title,
	UI,
	Combat,
	Dialogue
};

// Main Helbreath Application
class Helbreath : public core::Application {
public:
	Helbreath();
	~Helbreath() override = default;

	// Font management
	core::FontResource* GetFont(FontType type);
	core::FontManager<FontType>& GetFontManager() { return m_fontManager; }

protected:
	void OnInitialize() override;
	void OnShutdown() override;

private:
	core::FontManager<FontType> m_fontManager;
};
