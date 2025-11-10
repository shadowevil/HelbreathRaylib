#include "Helbreath.h"
#include "Game.h"

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
	// TODO: Replace these paths with actual font files
	// For now, these will fall back to the default font if the files don't exist
	m_fontManager.RegisterFont(FontType::Default, "assets/fonts/default.ttf", 32);
	m_fontManager.RegisterFont(FontType::Title, "assets/fonts/title.ttf", 48);
	m_fontManager.RegisterFont(FontType::UI, "assets/fonts/ui.ttf", 24);
	m_fontManager.RegisterFont(FontType::Combat, "assets/fonts/combat.ttf", 28);
	m_fontManager.RegisterFont(FontType::Dialogue, "assets/fonts/dialogue.ttf", 20);

	// Optional: Preload all fonts (comment out to use lazy loading with caching)
	// m_fontManager.LoadAll();
}

void Helbreath::OnShutdown() {
	// Font manager will automatically clean up in destructor
}

core::FontResource* Helbreath::GetFont(FontType type) {
	return m_fontManager.GetFont(type);
}
