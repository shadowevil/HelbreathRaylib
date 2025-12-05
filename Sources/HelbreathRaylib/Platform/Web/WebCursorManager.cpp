#include "WebCursorManager.h"
#include "../../Dependencies/Includes/raylib_include.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

WebCursorManager::WebCursorManager()
	: _isVisible(true), _isLocked(false) {
}

void WebCursorManager::hide() {
	// Don't use raylib::HideCursor() on web - it interferes with mouse input
	// Instead, use Emscripten to set CSS cursor style to 'none'
	_isVisible = false;
	_updateCursorStyle();
}

void WebCursorManager::show() {
	// Don't use raylib::ShowCursor() on web - see hide() comment
	// Instead, use Emscripten to set CSS cursor style to 'default'
	_isVisible = true;
	_updateCursorStyle();
}

void WebCursorManager::_updateCursorStyle() {
#ifdef __EMSCRIPTEN__
	if (_isVisible) {
		// Show hardware cursor
		EM_ASM({
			var canvas = document.getElementById('canvas');
			if (canvas) canvas.style.cursor = 'default';
		});
	} else {
		// Hide hardware cursor
		EM_ASM({
			var canvas = document.getElementById('canvas');
			if (canvas) canvas.style.cursor = 'none';
		});
	}
#endif
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
