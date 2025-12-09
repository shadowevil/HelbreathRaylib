#include "Scenes.h"
#include "Colors.h"

void MainMenuScene::on_initialize()
{
	// Button dimensions: height = 22, spacing = 15, start y = 178
	constexpr float button_x = 385.0f;
	constexpr float button_width = 164.0f;
	constexpr float button_height = 22.0f;
	constexpr float button_spacing = 15.0f;
	constexpr float start_y = 178.0f;

	// Login button
	UI::Button& login_button = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
	login_button.SetBounds(button_x, start_y, button_width, button_height);
	UI::Button::SpriteConfig login_cfg;
	login_cfg.sprite_id = SPRID_MAINMENUSCREEN;
	login_cfg.hover_frame = SPR_MAINMENUSCREEN::BUTTON_1;
	login_button.SetSprite(login_cfg);
	login_button.OnClick = [this](UI::Control*) {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		scene_manager.set_scene<CharacterSelectionScene>();
	};

	// New Account button
	UI::Button& new_account_button = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
	new_account_button.SetBounds(button_x, start_y + (button_height + button_spacing) + 1.0f, button_width, button_height);
	UI::Button::SpriteConfig new_account_cfg;
	new_account_cfg.sprite_id = SPRID_MAINMENUSCREEN;
	new_account_cfg.hover_frame = SPR_MAINMENUSCREEN::BUTTON_2;
	new_account_button.SetSprite(new_account_cfg);
	new_account_button.OnClick = [this](UI::Control*) {
		// TODO: New account logic
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
	};

	// Exit button
	UI::Button& exit_button = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
	exit_button.SetBounds(button_x, start_y + (button_height + button_spacing) * 2.0f + 3.0f, button_width, button_height);
	UI::Button::SpriteConfig exit_cfg;
	exit_cfg.sprite_id = SPRID_MAINMENUSCREEN;
	exit_cfg.hover_frame = SPR_MAINMENUSCREEN::BUTTON_3;
	exit_button.SetSprite(exit_cfg);
	exit_button.OnClick = [this](UI::Control*) {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		scene_manager.set_scene<ExitGameScene>();
	};
}

void MainMenuScene::on_uninitialize()
{
}

void MainMenuScene::on_update()
{
	for(auto& control : m_controls)
		control->Update();
}

void MainMenuScene::on_render()
{
	sprites[SPRID_MAINMENUSCREEN]->draw(0, 0, SPR_MAINMENUSCREEN::BACKGROUND);

	for (auto& control : m_controls)
		control->Render();
}
