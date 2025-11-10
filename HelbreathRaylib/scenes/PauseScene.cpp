#include "PauseScene.h"
#include "../Helbreath.h"

PauseScene::PauseScene()
	: Scene("Pause"), m_selectedOption(0) {
}

void PauseScene::OnEnter() {
	m_selectedOption = 0;
}

void PauseScene::OnExit() {
	// Cleanup
}

void PauseScene::Update(float deltaTime) {
	// Handle input
	if (IsKeyPressed(KEY_DOWN)) {
		m_selectedOption = (m_selectedOption + 1) % 2;
	}
	if (IsKeyPressed(KEY_UP)) {
		m_selectedOption = (m_selectedOption - 1 + 2) % 2;
	}
	if (IsKeyPressed(KEY_ENTER)) {
		// TODO: Handle pause menu selection
		// 0 = Resume, 1 = Quit to Menu
	}
	if (IsKeyPressed(KEY_ESCAPE)) {
		// Quick resume with ESC
		// TODO: Return to gameplay
	}
}

void PauseScene::Render() {
	int32_t screenWidth = GetScreenWidth();
	int32_t screenHeight = GetScreenHeight();

	// Draw semi-transparent overlay
	DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.7f));

	// Draw pause title
	const char* title = "PAUSED";
	int32_t titleFontSize = 48;
	int32_t titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, (screenWidth - titleWidth) / 2, 150, titleFontSize, RAYWHITE);

	// Draw menu options
	const char* options[] = { "Resume", "Quit to Menu" };
	int32_t menuY = screenHeight / 2;
	int32_t menuFontSize = 24;

	for (int32_t i = 0; i < 2; i++) {
		Color color = (i == m_selectedOption) ? YELLOW : LIGHTGRAY;
		int32_t textWidth = MeasureText(options[i], menuFontSize);
		DrawText(options[i], (screenWidth - textWidth) / 2, menuY + i * 40, menuFontSize, color);
	}

	// Draw instructions
	const char* instructions = "ESC to resume, UP/DOWN to select, ENTER to confirm";
	int32_t instructionsFontSize = 14;
	int32_t instructionsWidth = MeasureText(instructions, instructionsFontSize);
	DrawText(instructions, (screenWidth - instructionsWidth) / 2, screenHeight - 50, instructionsFontSize, GRAY);
}

void PauseScene::OnEvent(core::Event& event) {
	// Handle pause events
}
