#pragma once
// Includes for scenes
#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "ExitGameScene.h"

// Other necessary includes
#include "PAK.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "FontManager.h"
#include "Application.h"
#include "IDs.h"

// Helper functions

namespace core {
	template<typename T>
	inline bool RectangleContainsMouse(const rlx::Rectangle<T>& rect) {
		return rlx::RectangleContainsScaledMouse(rect, constant::BASE_WIDTH, constant::BASE_HEIGHT);
	}

	inline Vector2 GetMousePosition() {
		return rlx::GetScaledMousePosition(constant::BASE_WIDTH, constant::BASE_HEIGHT);
	}
}