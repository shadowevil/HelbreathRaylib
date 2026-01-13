#include "MessageBoxDialog.h"
#include "../SpriteIDs.h"
#include "../FontIDs.h"
#include "../FontSystem.h"
#include "../SoundIDs.h"
#include "../SoundPlayer.h"
#include "../global_constants.h"

MessageBoxDialog::MessageBoxDialog(const std::string& title, ButtonType button_type)
	: _title(title), _message(""), _button_type(button_type)
{
}

MessageBoxDialog::MessageBoxDialog(const std::string& title, const std::string& message, ButtonType button_type)
	: _title(title), _message(message), _button_type(button_type)
{
}

void MessageBoxDialog::on_initialize()
{
	// Center the dialog on screen
	CenterDialog(DIALOG_WIDTH, DIALOG_HEIGHT);

	// Create buttons based on button type
	CreateButtons();
}

void MessageBoxDialog::on_uninitialize()
{
	// Don't clear m_controls here - they will be cleaned up when the dialog
	// is destroyed. Clearing here causes use-after-free when a button's OnClick
	// callback triggers CloseWithResult, which pops the overlay while we're
	// still inside the button's Update method.
}

void MessageBoxDialog::on_update()
{
	for (auto& control : m_controls) {
		control->Update();
	}
}

void MessageBoxDialog::on_render()
{
	// Render semi-transparent overlay
	RenderOverlay();

	// Get dialog position
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Draw the dialog background sprite
	sprites[SPRID_DIALOG_MESSAGEBOX]->draw(
		static_cast<int>(dialog_x),
		static_cast<int>(dialog_y),
		SPR_DIALOG_MESSAGEBOX_INFO::BACKGROUND
	);

	// Draw title text (centered, Fancy font, size 20)
	if (!_title.empty()) {
		FontSystem::draw_text_aligned(
			FontFamily::Fancy, 20,
			_title.c_str(),
			dialog_x, dialog_y + TITLE_Y_OFFSET,
			DIALOG_WIDTH, 20.0f,
			raylib::WHITE,
			HorizontalAlign::Center | VerticalAlign::Top,
			FontStyle::Shadow
		);
	}

	// Draw message text (centered, Default font, size 16)
	if (!_message.empty()) {
		FontSystem::draw_text_aligned(
			FontFamily::Default, 16,
			_message.c_str(),
			dialog_x, dialog_y + MESSAGE_Y_OFFSET,
			DIALOG_WIDTH, 16.0f,
			raylib::WHITE,
			HorizontalAlign::Center | VerticalAlign::Top,
			FontStyle::Shadow
		);
	}

	// Render buttons
	for (auto& control : m_controls) {
		control->Render();
	}
}

void MessageBoxDialog::CreateButtons()
{
	switch (_button_type) {
	case ButtonType::Ok:
		CreateSingleButton("Ok", RESULT_OK);
		break;
	case ButtonType::OkCancel:
		CreateTwoButtons("Ok", RESULT_OK, "Cancel", RESULT_CANCEL);
		break;
	case ButtonType::YesNo:
		CreateTwoButtons("Yes", RESULT_YES, "No", RESULT_NO);
		break;
	case ButtonType::ContinueCancel:
		CreateTwoButtons("Continue", RESULT_CONTINUE, "Cancel", RESULT_CANCEL);
		break;
	}
}

void MessageBoxDialog::CreateSingleButton(const std::string& text, int result)
{
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Get button dimensions from sprite
	auto btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::MEDIUM_BUTTON);
	float btn_width = static_cast<float>(btn_rect.width);
	float btn_height = static_cast<float>(btn_rect.height);

	// Center the button horizontally
	float btn_x = dialog_x + (DIALOG_WIDTH - btn_width) / 2.0f;
	float btn_y = dialog_y + BUTTON_Y_OFFSET;

	auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
	btn.SetBounds(btn_x, btn_y, btn_width, btn_height);

	UI::Button::SpriteConfig sprite_cfg;
	sprite_cfg.sprite_id = SPRID_BUTTONS;
	sprite_cfg.normal_frame = SPR_BUTTONS::MEDIUM_BUTTON;
	sprite_cfg.hover_frame = SPR_BUTTONS::MEIDUM_BUTTON_HOVER;
	sprite_cfg.pressed_frame = SPR_BUTTONS::MEDIUM_BUTTON;
	sprite_cfg.disabled_frame = SPR_BUTTONS::MEDIUM_BUTTON;
	btn.SetSprite(sprite_cfg);

	UI::Button::TextConfig text_cfg;
	text_cfg.text = text;
	text_cfg.font_index = FontFamily::Default;
	text_cfg.font_size = 13;
	text_cfg.normal_color = raylib::WHITE;
	text_cfg.hover_color = raylib::YELLOW;
	text_cfg.pressed_color = raylib::GRAY;
	text_cfg.font_style = FontStyle::Shadow;
	text_cfg.h_align = HorizontalAlign::Center;
	text_cfg.v_align = VerticalAlign::Middle;
	btn.SetText(text_cfg);

	btn.OnClick = [this, result](UI::Control*) {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		CloseWithResult(result);
	};
}

void MessageBoxDialog::CreateTwoButtons(const std::string& left_text, int left_result,
										const std::string& right_text, int right_result)
{
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Get button dimensions from sprite
	auto btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::MEDIUM_BUTTON);
	float btn_width = static_cast<float>(btn_rect.width);
	float btn_height = static_cast<float>(btn_rect.height);

	// Calculate button positions (evenly spaced)
	float total_width = btn_width * 2 + BUTTON_MARGIN;
	float start_x = dialog_x + (DIALOG_WIDTH - total_width) / 2.0f;
	float btn_y = dialog_y + BUTTON_Y_OFFSET;

	// Left button
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
		text_cfg.text = left_text;
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 13;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this, left_result](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			CloseWithResult(left_result);
		};
	}

	// Right button
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
		text_cfg.text = right_text;
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 13;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this, right_result](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			CloseWithResult(right_result);
		};
	}
}
