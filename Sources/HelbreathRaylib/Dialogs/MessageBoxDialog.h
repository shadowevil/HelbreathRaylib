#pragma once
#include "OverlayDialog.h"
#include <string>

// MessageBox dialog with configurable buttons
// Supports: Yes/No, Continue/Cancel, Ok/Cancel, Ok only
class MessageBoxDialog : public OverlayDialog {
public:
	DIALOG_TYPE(MessageBoxDialog)

	// Button configurations
	enum class ButtonType {
		Ok,				// Single Ok button (centered)
		OkCancel,		// Ok and Cancel buttons
		YesNo,			// Yes and No buttons
		ContinueCancel	// Continue and Cancel buttons
	};

	// Result values returned via callback
	enum Result {
		RESULT_CANCEL = 0,
		RESULT_OK = 1,
		RESULT_YES = 1,
		RESULT_NO = 0,
		RESULT_CONTINUE = 1
	};

	// Constructor with message and button type
	MessageBoxDialog(const std::string& title, ButtonType button_type = ButtonType::Ok);
	MessageBoxDialog(const std::string& title, const std::string& message, ButtonType button_type = ButtonType::Ok);

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::string _title;
	std::string _message;
	ButtonType _button_type;

	// Dialog dimensions (from sprite: 270x104)
	static constexpr float DIALOG_WIDTH = 270.0f;
	static constexpr float DIALOG_HEIGHT = 104.0f;

	// Text positioning
	static constexpr float TITLE_Y_OFFSET = 20.0f;
	static constexpr float MESSAGE_Y_OFFSET = 45.0f;

	// Button positioning
	static constexpr float BUTTON_Y_OFFSET = 70.0f;
	static constexpr float BUTTON_MARGIN = 20.0f;

	void CreateButtons();
	void CreateSingleButton(const std::string& text, int result);
	void CreateTwoButtons(const std::string& left_text, int left_result,
						  const std::string& right_text, int right_result);
};
