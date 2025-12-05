#include "WebCursorManager.h"
#include "../../Dependencies/Includes/raylib_include.h"

WebCursorManager::WebCursorManager()
	: _isVisible(true), _isLocked(false) {
}

void WebCursorManager::hide() {
	// Don't use raylib::HideCursor() on web - it interferes with mouse input
	// Cursor is hidden via CSS (cursor: none) in shell.html instead
	_isVisible = false;
}

void WebCursorManager::show() {
	// Don't use raylib::ShowCursor() on web - see hide() comment
	// CSS handles cursor visibility
	_isVisible = true;
}

void WebCursorManager::lock() {
	// No-op on web - pointer lock removed for point-and-click gameplay
	// Cursor remains visible and free to move
}

void WebCursorManager::unlock() {
	// No-op on web
}

bool WebCursorManager::isLocked() const {
	return false;  // Never locked on web
}

bool WebCursorManager::isVisible() const {
	return _isVisible;
}
