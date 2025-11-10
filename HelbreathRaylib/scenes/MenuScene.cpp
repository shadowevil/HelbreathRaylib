#include "MenuScene.h"
#include "../Helbreath.h"

MenuScene::MenuScene()
	: Scene("Menu"), m_selectedOption(0), m_animationTime(0.0f) {
}

void MenuScene::OnEnter() {
	// Reset menu state
	m_selectedOption = 0;
	m_animationTime = 0.0f;
}

void MenuScene::OnExit() {
	// Cleanup if needed
}

void MenuScene::Update(float deltaTime) {
	m_animationTime += deltaTime;

	// Handle input
	if (IsKeyPressed(KEY_DOWN)) {
		m_selectedOption = (m_selectedOption + 1) % 3;
	}
	if (IsKeyPressed(KEY_UP)) {
		m_selectedOption = (m_selectedOption - 1 + 3) % 3;
	}
	if (IsKeyPressed(KEY_ENTER)) {
		// TODO: Get SceneManager from Helbreath and switch scenes
		// For now, this will be handled when we integrate it into Helbreath
	}
}

void MenuScene::Render() {
	int32_t screenWidth = GetScreenWidth();
	int32_t screenHeight = GetScreenHeight();

	// Draw title
	const char* title = "HELBREATH";
	int32_t titleFontSize = 48;
	int32_t titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, (screenWidth - titleWidth) / 2, 100, titleFontSize, RAYWHITE);

	// Draw menu options
	const char* options[] = { "Start Game", "Options", "Exit" };
	int32_t menuY = screenHeight / 2 - 50;
	int32_t menuFontSize = 24;

	for (int32_t i = 0; i < 3; i++) {
		Color color = (i == m_selectedOption) ? YELLOW : LIGHTGRAY;

		// Add pulsing effect to selected option
		if (i == m_selectedOption) {
			float pulse = (sinf(m_animationTime * 5.0f) + 1.0f) * 0.5f;
			color = ColorAlpha(YELLOW, 0.7f + pulse * 0.3f);
		}

		int32_t textWidth = MeasureText(options[i], menuFontSize);
		DrawText(options[i], (screenWidth - textWidth) / 2, menuY + i * 40, menuFontSize, color);
	}

	// Draw instructions
	const char* instructions = "Use UP/DOWN to select, ENTER to confirm";
	int32_t instructionsFontSize = 14;
	int32_t instructionsWidth = MeasureText(instructions, instructionsFontSize);
	DrawText(instructions, (screenWidth - instructionsWidth) / 2, screenHeight - 50, instructionsFontSize, GRAY);
}

void MenuScene::OnEvent(core::Event& event) {
	// Handle events if needed
	core::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<core::KeyPressedEvent>([this](core::KeyPressedEvent& e) {
		if (e.GetKeyCode() == KEY_ESCAPE) {
			// Close application from menu
			core::Application::Get().Close();
			return true;
		}
		return false;
	});
}
