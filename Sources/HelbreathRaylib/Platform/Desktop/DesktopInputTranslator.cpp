#include "DesktopInputTranslator.h"
#include "../../Dependencies/Includes/raylib_include.h"

raylib::Vector2 DesktopInputTranslator::getMousePosition() {
	return raylib::GetMousePosition();
}

bool DesktopInputTranslator::isMouseButtonPressed(int button) {
	return raylib::IsMouseButtonPressed(button);
}

bool DesktopInputTranslator::isMouseButtonDown(int button) {
	return raylib::IsMouseButtonDown(button);
}

bool DesktopInputTranslator::isKeyPressed(int key) {
	return raylib::IsKeyPressed(key);
}

bool DesktopInputTranslator::isKeyDown(int key) {
	return raylib::IsKeyDown(key);
}

void DesktopInputTranslator::processInput() {
	// No special input processing needed on desktop
	// Native keyboard shortcuts work normally
}
