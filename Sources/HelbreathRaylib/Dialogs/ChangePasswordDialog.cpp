#include "ChangePasswordDialog.h"
#include "../SpriteIDs.h"
#include "../FontIDs.h"
#include "../FontSystem.h"
#include "../SoundIDs.h"
#include "../SoundPlayer.h"
#include "../global_constants.h"

ChangePasswordDialog::ChangePasswordDialog()
{
}

void ChangePasswordDialog::on_initialize()
{
	// Center the dialog on screen
	CenterDialog(DIALOG_WIDTH, DIALOG_HEIGHT);

	// Create input boxes and buttons
	CreateInputBoxes();
	CreateButtons();
}

void ChangePasswordDialog::on_uninitialize()
{
	// Just null out the pointers - don't clear m_controls here as it causes
	// use-after-free when a button callback triggers CloseWithResult.
	// Controls will be cleaned up when the dialog is destroyed.
	_current_password_input = nullptr;
	_new_password_input = nullptr;
	_confirm_password_input = nullptr;
}

void ChangePasswordDialog::on_update()
{
	for (auto& control : m_controls) {
		control->Update();
	}
}

void ChangePasswordDialog::on_render()
{
	// Render semi-transparent overlay
	RenderOverlay();

	// Get dialog position
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Draw the dialog background sprite
	sprites[SPRID_DIALOG_PASSWORD]->draw(
		static_cast<int>(dialog_x),
		static_cast<int>(dialog_y),
		SPR_DIALOG_PASSWORD_INFO::BACKGROUND
	);

	// Draw title
	FontSystem::draw_text_aligned(
		FontFamily::Fancy, 20,
		"Change Password",
		dialog_x, dialog_y + TITLE_Y,
		DIALOG_WIDTH, 24.0f,
		raylib::WHITE,
		HorizontalAlign::Center | VerticalAlign::Top,
		FontStyle::Shadow
	);

	// Draw labels
	float label_y = dialog_y + FIRST_ROW_Y;

	FontSystem::draw_text_aligned(
		FontFamily::Default, 14,
		"Current:",
		dialog_x + LABEL_X, label_y,
		100.0f, INPUT_HEIGHT,
		raylib::WHITE,
		HorizontalAlign::Left | VerticalAlign::Middle,
		FontStyle::Shadow
	);

	label_y += ROW_HEIGHT;
	FontSystem::draw_text_aligned(
		FontFamily::Default, 14,
		"New:",
		dialog_x + LABEL_X, label_y,
		100.0f, INPUT_HEIGHT,
		raylib::WHITE,
		HorizontalAlign::Left | VerticalAlign::Middle,
		FontStyle::Shadow
	);

	label_y += ROW_HEIGHT;
	FontSystem::draw_text_aligned(
		FontFamily::Default, 14,
		"Confirm:",
		dialog_x + LABEL_X, label_y,
		100.0f, INPUT_HEIGHT,
		raylib::WHITE,
		HorizontalAlign::Left | VerticalAlign::Middle,
		FontStyle::Shadow
	);

	// Render controls
	for (auto& control : m_controls) {
		control->Render();
	}
}

std::string ChangePasswordDialog::GetCurrentPassword() const
{
	return _current_password_input ? _current_password_input->GetText() : "";
}

std::string ChangePasswordDialog::GetNewPassword() const
{
	return _new_password_input ? _new_password_input->GetText() : "";
}

void ChangePasswordDialog::CreateInputBoxes()
{
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	UI::InputBox::Config input_cfg;
	input_cfg.password_mode = true;
	input_cfg.max_length = 16;
	input_cfg.font_index = FontFamily::Default;
	input_cfg.font_size = 13;
	input_cfg.background_color = raylib::Color{ 20, 20, 20, 200 };
	input_cfg.border_color = raylib::Color{ 80, 80, 80, 255 };
	input_cfg.focused_border_color = raylib::Color{ 150, 150, 150, 255 };
	input_cfg.border_thickness = 1.0f;
	input_cfg.padding_left = 5.0f;
	input_cfg.padding_right = 5.0f;

	float input_y = dialog_y + FIRST_ROW_Y;

	// Current password input
	{
		auto& input = static_cast<UI::InputBox&>(*m_controls.emplace_back(std::make_unique<UI::InputBox>()).get());
		_current_password_input = &input;
		input.SetBounds(dialog_x + INPUT_X, input_y, INPUT_WIDTH, INPUT_HEIGHT);
		input.SetConfig(input_cfg);
	}

	input_y += ROW_HEIGHT;

	// New password input
	{
		auto& input = static_cast<UI::InputBox&>(*m_controls.emplace_back(std::make_unique<UI::InputBox>()).get());
		_new_password_input = &input;
		input.SetBounds(dialog_x + INPUT_X, input_y, INPUT_WIDTH, INPUT_HEIGHT);
		input.SetConfig(input_cfg);
	}

	input_y += ROW_HEIGHT;

	// Confirm password input
	{
		auto& input = static_cast<UI::InputBox&>(*m_controls.emplace_back(std::make_unique<UI::InputBox>()).get());
		_confirm_password_input = &input;
		input.SetBounds(dialog_x + INPUT_X, input_y, INPUT_WIDTH, INPUT_HEIGHT);
		input.SetConfig(input_cfg);
	}
}

void ChangePasswordDialog::CreateButtons()
{
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Get button dimensions from sprite
	auto btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::MEDIUM_BUTTON);
	float btn_width = static_cast<float>(btn_rect.width);
	float btn_height = static_cast<float>(btn_rect.height);

	// Calculate button positions
	float total_width = btn_width * 2 + BUTTON_MARGIN;
	float start_x = dialog_x + (DIALOG_WIDTH - total_width) / 2.0f;
	float btn_y = dialog_y + BUTTON_Y;

	// OK button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(start_x, btn_y, btn_width, btn_height);

		UI::Button::SpriteConfig sprite_cfg;
		sprite_cfg.sprite_id = SPRID_BUTTONS;
		sprite_cfg.normal_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		sprite_cfg.hover_frame = SPR_BUTTONS::MEIDUM_BUTTON_HOVER;
		sprite_cfg.pressed_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		sprite_cfg.disabled_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		btn.SetSprite(sprite_cfg);

		UI::Button::TextConfig text_cfg;
		text_cfg.text = "Ok";
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 13;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this](UI::Control*) {
			if (ValidatePasswords()) {
				sound_player.play_effect_multi(Sound::BUTTON_SOUND);
				CloseWithResult(RESULT_OK);
			} else {
				sound_player.play_effect_multi(Sound::FAILURE);
			}
		};
	}

	// Cancel button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(start_x + btn_width + BUTTON_MARGIN, btn_y, btn_width, btn_height);

		UI::Button::SpriteConfig sprite_cfg;
		sprite_cfg.sprite_id = SPRID_BUTTONS;
		sprite_cfg.normal_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		sprite_cfg.hover_frame = SPR_BUTTONS::MEIDUM_BUTTON_HOVER;
		sprite_cfg.pressed_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		sprite_cfg.disabled_frame = SPR_BUTTONS::MEDIUM_BUTTON;
		btn.SetSprite(sprite_cfg);

		UI::Button::TextConfig text_cfg;
		text_cfg.text = "Cancel";
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 13;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			CloseWithResult(RESULT_CANCEL);
		};
	}
}

bool ChangePasswordDialog::ValidatePasswords()
{
	if (!_new_password_input || !_confirm_password_input) {
		return false;
	}

	// Check that new password matches confirmation
	return _new_password_input->GetText() == _confirm_password_input->GetText() &&
		   !_new_password_input->GetText().empty();
}
