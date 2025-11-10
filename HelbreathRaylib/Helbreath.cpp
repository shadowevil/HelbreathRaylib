#include "Helbreath.h"
#include "Game.h"

Helbreath::Helbreath()
	: Application(core::WindowProperties("Helbreath Raylib", 800, 600, true, true, false))
{
	// Push game layer
	PushLayer(std::make_unique<Game>());
}

void Helbreath::OnInitialize() {
	// Custom initialization code here
}

void Helbreath::OnShutdown() {
	// Custom shutdown code here
}
