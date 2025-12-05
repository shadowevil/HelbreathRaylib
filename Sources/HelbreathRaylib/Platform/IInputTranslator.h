#pragma once
#include "raylib_include.h"

/**
 * Input translation and normalization abstraction.
 * Handles platform-specific input quirks transparently.
 *
 * Desktop: Pass-through to raylib, native shortcuts work
 * Web: Blocks browser shortcuts (Ctrl+T, Ctrl+W, F-keys), handles canvas coordinates
 */
class IInputTranslator {
public:
	virtual ~IInputTranslator() = default;

	// Mouse input
	virtual raylib::Vector2 getMousePosition() = 0;
	virtual bool isMouseButtonPressed(int button) = 0;
	virtual bool isMouseButtonDown(int button) = 0;

	// Keyboard input
	virtual bool isKeyPressed(int key) = 0;
	virtual bool isKeyDown(int key) = 0;

	// Platform-specific input processing (called once per frame)
	// Desktop: no-op
	// Web: blocks browser shortcuts, prevents navigation
	virtual void processInput() = 0;
};
