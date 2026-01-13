#pragma once
#include "OverlayDialog.h"
#include "../UI/InputBox.h"
#include <string>

// Change Password dialog - placeholder for future implementation
// Currently not implemented as this is a single-player game
class ChangePasswordDialog : public OverlayDialog {
public:
	DIALOG_TYPE(ChangePasswordDialog)

	// Result values
	enum Result {
		RESULT_CANCEL = 0,
		RESULT_OK = 1
	};

	ChangePasswordDialog();

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

	// Get the entered passwords (only valid after RESULT_OK)
	std::string GetCurrentPassword() const;
	std::string GetNewPassword() const;

private:
	UI::InputBox* _current_password_input = nullptr;
	UI::InputBox* _new_password_input = nullptr;
	UI::InputBox* _confirm_password_input = nullptr;

	// Dialog dimensions (from sprite: 334x256)
	static constexpr float DIALOG_WIDTH = 334.0f;
	static constexpr float DIALOG_HEIGHT = 256.0f;

	// Layout constants
	static constexpr float TITLE_Y = 20.0f;
	static constexpr float LABEL_X = 30.0f;
	static constexpr float INPUT_X = 140.0f;
	static constexpr float INPUT_WIDTH = 160.0f;
	static constexpr float INPUT_HEIGHT = 22.0f;
	static constexpr float ROW_HEIGHT = 40.0f;
	static constexpr float FIRST_ROW_Y = 70.0f;
	static constexpr float BUTTON_Y = 200.0f;
	static constexpr float BUTTON_MARGIN = 20.0f;

	void CreateInputBoxes();
	void CreateButtons();
	bool ValidatePasswords();
};
