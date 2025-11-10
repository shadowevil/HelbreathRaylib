#include "GameplayScene.h"
#include "../Helbreath.h"

GameplayScene::GameplayScene()
	: Scene("Gameplay"), m_gameTime(0.0f), m_paused(false) {
}

void GameplayScene::OnEnter() {
	// Initialize game state
	m_gameTime = 0.0f;
	m_paused = false;
}

void GameplayScene::OnExit() {
	// Save game state or cleanup
}

void GameplayScene::Update(float deltaTime) {
	if (!m_paused) {
		m_gameTime += deltaTime;
	}

	// Check for pause
	if (IsKeyPressed(KEY_ESCAPE)) {
		// TODO: Transition to pause scene
		// For now, go back to menu with fade transition
	}
}

void GameplayScene::Render() {
	int32_t screenWidth = GetScreenWidth();
	int32_t screenHeight = GetScreenHeight();

	// Draw gameplay placeholder
	DrawText("GAMEPLAY SCENE", 50, 50, 32, RAYWHITE);

	// Draw game time
	char timeText[64];
	snprintf(timeText, sizeof(timeText), "Game Time: %.1f seconds", m_gameTime);
	DrawText(timeText, 50, 100, 20, LIGHTGRAY);

	// Draw instructions
	DrawText("ESC - Return to Menu", 50, screenHeight - 50, 16, GRAY);

	// Draw some visual feedback
	float x = (sinf(m_gameTime) + 1.0f) * 0.5f * (screenWidth - 100);
	float y = (cosf(m_gameTime) + 1.0f) * 0.5f * (screenHeight - 100);
	DrawCircle(static_cast<int32_t>(x + 50), static_cast<int32_t>(y + 50), 20.0f, YELLOW);

	if (m_paused) {
		// Draw pause overlay
		DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.5f));
		const char* pausedText = "PAUSED";
		int32_t textWidth = MeasureText(pausedText, 48);
		DrawText(pausedText, (screenWidth - textWidth) / 2, screenHeight / 2 - 24, 48, YELLOW);
	}
}

void GameplayScene::OnEvent(core::Event& event) {
	// Handle gameplay events
	core::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<core::KeyPressedEvent>([this](core::KeyPressedEvent& e) {
		if (e.GetKeyCode() == KEY_P) {
			m_paused = !m_paused;
			return true;
		}
		return false;
	});
}
