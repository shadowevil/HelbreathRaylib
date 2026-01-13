#pragma once
#include "OverlayDialog.h"
#include <string>

// Drop Item dialog - placeholder for future implementation
// Will support Yes/No buttons for confirming item drops
class DropItemDialog : public OverlayDialog {
public:
	DIALOG_TYPE(DropItemDialog)

	// Result values
	enum Result {
		RESULT_NO = 0,
		RESULT_YES = 1
	};

	// Constructor with item name to drop
	DropItemDialog(const std::string& item_name);

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::string _item_name;

	// Dialog dimensions (from sprite: 215x87)
	static constexpr float DIALOG_WIDTH = 215.0f;
	static constexpr float DIALOG_HEIGHT = 87.0f;

	// Text positioning
	static constexpr float TEXT_Y_OFFSET = 15.0f;

	// Button positioning
	static constexpr float BUTTON_Y_OFFSET = 50.0f;
	static constexpr float BUTTON_MARGIN = 15.0f;

	void CreateButtons();
};
