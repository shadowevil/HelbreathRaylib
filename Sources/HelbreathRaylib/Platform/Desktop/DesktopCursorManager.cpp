#include "DesktopCursorManager.h"
#include "../../Dependencies/Includes/raylib_include.h"
#include "../../Dependencies/Includes/render_utils.h"

DesktopCursorManager::DesktopCursorManager()
	: _isVisible(true), _isLocked(false), _gameWidth(0), _gameHeight(0) {
}

void DesktopCursorManager::hide() {
	if (_isVisible) {
		raylib::HideCursor();
		_isVisible = false;
	}
}

void DesktopCursorManager::show() {
	if (!_isVisible) {
		raylib::ShowCursor();
		_isVisible = true;
	}
}

void DesktopCursorManager::lock() {
	if (!_isLocked && _gameWidth > 0 && _gameHeight > 0) {
		rlx::LockCursor(_gameWidth, _gameHeight);
		_isLocked = true;
	}
}

void DesktopCursorManager::unlock() {
	if (_isLocked) {
#ifdef _WIN32
		windows::ClipCursor(nullptr);  // Unlock cursor on Windows
#endif
		_isLocked = false;
	}
}

bool DesktopCursorManager::isLocked() const {
	return _isLocked;
}

bool DesktopCursorManager::isVisible() const {
	return _isVisible;
}

void DesktopCursorManager::setGameArea(int width, int height) {
	_gameWidth = width;
	_gameHeight = height;
}
