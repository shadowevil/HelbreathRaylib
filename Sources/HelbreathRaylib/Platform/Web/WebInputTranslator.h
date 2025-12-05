#pragma once
#include "../IInputTranslator.h"

/**
 * Web implementation of input translation.
 * Pass-through to raylib input functions.
 * Browser shortcut blocking is handled at HTML/JS level in shell.html.
 */
class WebInputTranslator : public IInputTranslator {
public:
	WebInputTranslator() = default;
	~WebInputTranslator() override = default;

	raylib::Vector2 getMousePosition() override;
	bool isMouseButtonPressed(int button) override;
	bool isMouseButtonDown(int button) override;
	bool isKeyPressed(int key) override;
	bool isKeyDown(int key) override;
	void processInput() override;
};
