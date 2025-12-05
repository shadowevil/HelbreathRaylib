#pragma once
#include "../ICursorManager.h"

/**
 * Web implementation of cursor management.
 * Uses raylib's cursor functions for show/hide.
 * Lock/unlock are no-ops (pointer lock removed for point-and-click gameplay).
 */
class WebCursorManager : public ICursorManager {
public:
	WebCursorManager();
	~WebCursorManager() override = default;

	void hide() override;
	void show() override;
	void lock() override;    // No-op on web
	void unlock() override;  // No-op on web
	bool isLocked() const override;
	bool isVisible() const override;

private:
	bool _isVisible;
	bool _isLocked;  // Always false on web
};
