#include "DropItemDialog.h"
#include "../SpriteIDs.h"
#include "../FontIDs.h"
#include "../FontSystem.h"
#include "../SoundIDs.h"
#include "../SoundPlayer.h"
#include "../global_constants.h"

DropItemDialog::DropItemDialog(const std::string& item_name)
	: _item_name(item_name)
{
}

void DropItemDialog::on_initialize()
{
	// Center the dialog on screen
	CenterDialog(DIALOG_WIDTH, DIALOG_HEIGHT);

	// Create Yes/No buttons
	CreateButtons();
}

void DropItemDialog::on_uninitialize()
{
	// Don't clear m_controls here - they will be cleaned up when the dialog
	// is destroyed. Clearing here causes use-after-free when a button's OnClick
	// callback triggers CloseWithResult.
}

void DropItemDialog::on_update()
{
	for (auto& control : m_controls) {
		control->Update();
	}
}

void DropItemDialog::on_render()
{
	// Render semi-transparent overlay
	RenderOverlay();

	// Get dialog position
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Draw the dialog background sprite
	sprites[SPRID_DIALOG_MISC]->draw(
		static_cast<int>(dialog_x),
		static_cast<int>(dialog_y),
		SPR_DIALOG_MISC_INFO::DROPITEM
	);

	// Draw prompt text
	std::string prompt = "Drop " + _item_name + "?";
	FontSystem::draw_text_aligned(
		FontFamily::Fancy, 16,
		prompt.c_str(),
		dialog_x, dialog_y + TEXT_Y_OFFSET,
		DIALOG_WIDTH, 20.0f,
		raylib::WHITE,
		HorizontalAlign::Center | VerticalAlign::Top,
		FontStyle::Shadow
	);

	// Render buttons
	for (auto& control : m_controls) {
		control->Render();
	}
}

void DropItemDialog::CreateButtons()
{
	float dialog_x = GetDialogX();
	float dialog_y = GetDialogY();

	// Get button dimensions from sprite (use small buttons for this dialog)
	auto btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::SMALL_BUTTON);
	float btn_width = static_cast<float>(btn_rect.width);
	float btn_height = static_cast<float>(btn_rect.height);

	// Calculate button positions (evenly spaced)
	float total_width = btn_width * 2 + BUTTON_MARGIN;
	float start_x = dialog_x + (DIALOG_WIDTH - total_width) / 2.0f;
	float btn_y = dialog_y + BUTTON_Y_OFFSET;

	// Yes button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(start_x, btn_y, btn_width, btn_height);

		UI::Button::SpriteConfig sprite_cfg;
		sprite_cfg.sprite_id = SPRID_BUTTONS;
		sprite_cfg.normal_frame = SPR_BUTTONS::SMALL_BUTTON;
		sprite_cfg.hover_frame = SPR_BUTTONS::SMALL_BUTTON_HOVER;
		sprite_cfg.pressed_frame = SPR_BUTTONS::SMALL_BUTTON;
		sprite_cfg.disabled_frame = SPR_BUTTONS::SMALL_BUTTON;
		btn.SetSprite(sprite_cfg);

		UI::Button::TextConfig text_cfg;
		text_cfg.text = "Yes";
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 11;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			CloseWithResult(RESULT_YES);
		};
	}

	// No button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(start_x + btn_width + BUTTON_MARGIN, btn_y, btn_width, btn_height);

		UI::Button::SpriteConfig sprite_cfg;
		sprite_cfg.sprite_id = SPRID_BUTTONS;
		sprite_cfg.normal_frame = SPR_BUTTONS::SMALL_BUTTON;
		sprite_cfg.hover_frame = SPR_BUTTONS::SMALL_BUTTON_HOVER;
		sprite_cfg.pressed_frame = SPR_BUTTONS::SMALL_BUTTON;
		sprite_cfg.disabled_frame = SPR_BUTTONS::SMALL_BUTTON;
		btn.SetSprite(sprite_cfg);

		UI::Button::TextConfig text_cfg;
		text_cfg.text = "No";
		text_cfg.font_index = FontFamily::Default;
		text_cfg.font_size = 11;
		text_cfg.normal_color = raylib::WHITE;
		text_cfg.hover_color = raylib::YELLOW;
		text_cfg.pressed_color = raylib::GRAY;
		text_cfg.font_style = FontStyle::Shadow;
		text_cfg.h_align = HorizontalAlign::Center;
		text_cfg.v_align = VerticalAlign::Middle;
		btn.SetText(text_cfg);

		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			CloseWithResult(RESULT_NO);
		};
	}
}
