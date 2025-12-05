#include "WebInputTranslator.h"
#include "../../Dependencies/Includes/raylib_include.h"

raylib::Vector2 WebInputTranslator::getMousePosition() {
	return raylib::GetMousePosition();
}

bool WebInputTranslator::isMouseButtonPressed(int button) {
	return raylib::IsMouseButtonPressed(button);
}

bool WebInputTranslator::isMouseButtonDown(int button) {
	return raylib::IsMouseButtonDown(button);
}

bool WebInputTranslator::isKeyPressed(int key) {
	return raylib::IsKeyPressed(key);
}

bool WebInputTranslator::isKeyDown(int key) {
	return raylib::IsKeyDown(key);
}

void WebInputTranslator::processInput() {
	// Browser shortcut blocking is handled at HTML/JS level (shell.html)
	// - Blocks ALL function keys (F1-F12)
	// - Blocks ALL Ctrl/Cmd combinations (Ctrl+T, Ctrl+N, Ctrl+W, etc.)
	// - Blocks Alt combinations
	// No C++ processing needed
}
