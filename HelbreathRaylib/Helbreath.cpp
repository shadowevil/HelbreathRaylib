#include "Helbreath.h"
#include "Game.h"
#include "scenes/MenuScene.h"
#include "scenes/GameplayScene.h"
#include "scenes/PauseScene.h"

Helbreath::Helbreath()
	: Application(core::WindowProperties("Helbreath Raylib", 800, 600,
		core::WINDOW_VSYNC | core::WINDOW_RESIZABLE)),
	  m_fontManager(30.0f) // 30 second cache timeout
{
	// Push game layer
	PushLayer<Game>();
}

void Helbreath::OnInitialize() {
	// Register fonts with paths and sizes
	m_fontManager.RegisterFont(FontType::Default, "C:/Windows/Fonts/Tahoma.ttf", 14);
	m_fontManager.RegisterFont(FontType::Title, "C:/Windows/Fonts/Arial.ttf", 24);
	m_fontManager.RegisterFont(FontType::UI, "C:/Windows/Fonts/Tahoma.ttf", 12);
	m_fontManager.RegisterFont(FontType::Combat, "C:/Windows/Fonts/Arial.ttf", 16);
	m_fontManager.RegisterFont(FontType::Dialogue, "C:/Windows/Fonts/Tahoma.ttf", 14);

	// Optional: Preload all fonts (comment out to use lazy loading with caching)
	// m_fontManager.LoadAll();

	// Setup scenes to demonstrate scene transitions
	// Comment this out if you want to use the Layer system instead
	m_sceneManager.AddScene<MenuScene>("menu");
	m_sceneManager.AddScene<GameplayScene>("gameplay");
	m_sceneManager.AddScene<PauseScene>("pause");

	// Start with menu scene (with fade transition)
	// m_sceneManager.SwitchTo("menu", core::SceneTransition::Fade);
}

void Helbreath::OnShutdown() {
	// Font manager will automatically clean up in destructor
}

core::FontResource* Helbreath::GetFont(FontType type) {
	return m_fontManager.GetFont(type);
}
