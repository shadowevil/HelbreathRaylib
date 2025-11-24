#include <memory>
#include "Helbreath.h"
#include "Game.h"

Helbreath* Helbreath::s_Instance = nullptr;

Helbreath::Helbreath()
	: Application(core::Window::Config{
		"Helbreath Raylib",
		800, 600,
		FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT,
		0
		}) {
}

Helbreath::~Helbreath() {
	StopPulse();
}

void Helbreath::InitializeComponents() {
	s_Instance = this;

	m_pGame = PushLayer<Game>();
	StartPulse();
}

void Helbreath::FocusLost()
{
	rlx::UnlockCursor();
}

void Helbreath::FocusGained()
{
	rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);
}

void Helbreath::PulseLoop() {
	while (running) {
		// Perform periodic tasks here
		m_pGame->OnPulse();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}