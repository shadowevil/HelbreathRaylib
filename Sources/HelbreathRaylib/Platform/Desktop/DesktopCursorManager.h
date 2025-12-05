#pragma once
#include "../ICursorManager.h"

/**
 * Desktop implementation of cursor management.
 * Uses raylib's native cursor functions and Windows cursor locking.
 */
class DesktopCursorManager : public ICursorManager {
public:
	DesktopCursorManager();
	~DesktopCursorManager() override = default;

	void hide() override;
	void show() override;
	void lock() override;
	void unlock() override;
	bool isLocked() const override;
	bool isVisible() const override;

	// Set the game area dimensions for cursor locking
	void setGameArea(int width, int height);

private:
	bool _isVisible;
	bool _isLocked;
	int _gameWidth;
	int _gameHeight;
};
