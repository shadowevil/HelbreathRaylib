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
	m_fontManager.RegisterFont(FontType::Default, "C:/Windows/Fonts/Tahoma.ttf", 14);

	// m_fontManager.LoadAll();
}

void Helbreath::OnShutdown() {
	// Font manager will automatically clean up in destructor
}

core::FontResource* Helbreath::GetFont(FontType type) {
	return m_fontManager.GetFont(type);
}
