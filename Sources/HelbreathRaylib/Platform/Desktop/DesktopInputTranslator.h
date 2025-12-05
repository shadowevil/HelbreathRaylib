#pragma once
#include "../IInputTranslator.h"

/**
 * Desktop implementation of input translation.
 * Pass-through to raylib input functions - no special handling needed.
 */
class DesktopInputTranslator : public IInputTranslator {
public:
	DesktopInputTranslator() = default;
	~DesktopInputTranslator() override = default;

	raylib::Vector2 getMousePosition() override;
	bool isMouseButtonPressed(int button) override;
	bool isMouseButtonDown(int button) override;
	bool isKeyPressed(int key) override;
	bool isKeyDown(int key) override;
	void processInput() override;
};
