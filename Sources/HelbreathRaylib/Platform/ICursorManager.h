#pragma once

/**
 * Cursor management abstraction.
 * Hides platform-specific cursor behavior (native vs CSS).
 *
 * Desktop: Uses raylib HideCursor/ShowCursor and actual pointer lock
 * Web: Uses CSS cursor properties, lock() is no-op
 */
class ICursorManager {
public:
	virtual ~ICursorManager() = default;

	// Show/hide cursor
	virtual void hide() = 0;
	virtual void show() = 0;

	// Lock cursor to window (Desktop: real lock, Web: no-op or CSS)
	virtual void lock() = 0;
	virtual void unlock() = 0;

	// Query cursor state
	virtual bool isLocked() const = 0;
	virtual bool isVisible() const = 0;
};
