#pragma once
#include "raylib_include.h"
#include "core/Application.h"
#include "core/Font.h"
#include "core/Scene.h"

// Main Helbreath Application
class Helbreath : public core::Application {
public:
	// Font types for the Helbreath application
	enum class FontType : uint16_t {
		Default = 0,
		Title,
		UI,
		Combat,
		Dialogue
	};

	Helbreath();
	~Helbreath() override = default;

	// Font management
	core::FontResource* GetFont(FontType type);
	core::FontManager<FontType>& GetFontManager() { return m_fontManager; }

	// Scene management
	core::SceneManager& GetSceneManager() { return m_sceneManager; }

protected:
	void OnInitialize() override;
	void OnShutdown() override;

private:
	core::FontManager<FontType> m_fontManager;
	core::SceneManager m_sceneManager;
};
