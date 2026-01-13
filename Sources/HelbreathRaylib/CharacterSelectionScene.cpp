#include "Scenes.h"
#include "json.hpp"
#include <memory>

void CharacterSelectionScene::on_initialize()
{
	if(!rlx::Directory::Exists(constant::SAVE_GAME_PATH))
	{
		rlx::Directory::Create(constant::SAVE_GAME_PATH);
	}

	// Configure static entity behavior
	StaticEntityManager::EntityConfig config;
	config.animation_type = AnimationType::WALK;
	config.weapon_used = WeaponUsed::HAND;
	config.direction_change_min_interval = 5.0;
	config.direction_change_max_interval = 10.0;
	config.randomize_direction = true;

	using namespace position_literals;
	using json = nlohmann::json;

	// Get slot dimensions for character positioning
	auto slot_rect = sprites[SPRID_CHARACTERSELECTSCREEN]->get_frame_rectangle(SPR_CHARACTERSELECTSCREEN::SELECTED_CHARACTER);
	const float slot_width = static_cast<float>(slot_rect.width);
	const float slot_start_x = 103.0f;
	const float slot_start_y = 54.0f;
	const float slot_gap = 2.0f;
	const float character_offset_y = 98.0f;  // Offset from slot top to character center

	auto SaveFiles = rlx::Directory::GetFilesWithExtension(constant::SAVE_GAME_PATH, ".jsave");
	for(auto& FilePath : SaveFiles)
	{
		// Limit to MAX_CHARACTER_SLOTS (4) save files
		if (_character_list.size() >= MAX_CHARACTER_SLOTS) break;
		try {
			auto JsaveData = json::parse(rlx::File::ReadAllText(FilePath));

			// Extract name from root level
			std::string character_name = JsaveData.value("name", "Unknown");

			// Extract appearance and stats from JSON
			if (JsaveData.contains("player")) {
				auto& player_data = JsaveData["player"];

				PlayerAppearance appearance;
				if (player_data.contains("appearance")) {
					appearance = player_data["appearance"].get<PlayerAppearance>();
				}

				PlayerEntityStats stats;
				if (player_data.contains("stats")) {
					stats = player_data["stats"].get<PlayerEntityStats>();
				}

				// Calculate position within the character's slot card
				size_t slot_index = _character_list.size();
				float char_x = slot_start_x + (slot_index * (slot_width + slot_gap)) + (slot_width / 2.0f);
				float char_y = slot_start_y + character_offset_y;

				// Create player and add to list
				Player* player = _character_list.emplace_back(std::make_unique<Player>(appearance, stats)).get();
				_character_names.emplace_back(std::move(character_name));
				static_entity_manager.AddPlayer(player, GamePosition(PixelCoord(static_cast<uint32_t>(char_x)), PixelCoord(static_cast<uint32_t>(char_y))), appearance, config);
			}
		}
		catch (const json::exception&) {
			continue;
		}
	}

	// Create character slot toggle buttons
	{
		auto slot_rect = sprites[SPRID_CHARACTERSELECTSCREEN]->get_frame_rectangle(SPR_CHARACTERSELECTSCREEN::SELECTED_CHARACTER);
		const float slot_width = static_cast<float>(slot_rect.width);
		const float slot_height = static_cast<float>(slot_rect.height);
		const float slot_start_x = 103.0f;
		const float slot_start_y = 54.0f;
		const float slot_gap = 2.0f;

		for (size_t i = 0; i < MAX_CHARACTER_SLOTS; ++i) {
			auto& slot_btn = static_cast<UI::ToggleButton&>(*m_controls.emplace_back(std::make_unique<UI::ToggleButton>(sprites)).get());
			_slot_buttons[i] = &slot_btn;

			float slot_x = slot_start_x + static_cast<float>(i) * (slot_width + slot_gap);
			slot_btn.SetBounds(slot_x, slot_start_y, slot_width, slot_height);

			// No normal frame (baked into background), only show selected sprite when toggled/hovered
			UI::Button::SpriteConfig slot_sprite_cfg;
			slot_sprite_cfg.sprite_id = SPRID_CHARACTERSELECTSCREEN;
			slot_sprite_cfg.normal_frame = -1;  // No sprite when not selected
			slot_sprite_cfg.hover_frame = SPR_CHARACTERSELECTSCREEN::SELECTED_CHARACTER;
			slot_sprite_cfg.pressed_frame = SPR_CHARACTERSELECTSCREEN::SELECTED_CHARACTER;
			slot_sprite_cfg.disabled_frame = -1;
			slot_btn.SetSprite(slot_sprite_cfg);

			// Capture slot index for the callback
			int slot_index = static_cast<int>(i);
			slot_btn.OnClick = [this, slot_index](UI::Control*) {
				float current_time = static_cast<float>(raylib::GetTime());

				// Check for double-click
				if (_last_clicked_slot == slot_index &&
					(current_time - _last_click_time) <= DOUBLE_CLICK_TIME) {
					// Double-click detected
					bool is_empty_slot = static_cast<size_t>(slot_index) >= _character_list.size();
					if (is_empty_slot) {
						// Double-click on empty slot - create new character
						if (_character_list.size() < MAX_CHARACTER_SLOTS) {
							sound_player.play_effect_multi(Sound::BUTTON_SOUND);
							scene_manager.set_scene<CreateCharacterScene>();
							return;
						}
					}
					else {
						// Double-click on character slot - enter game
						sound_player.play_effect_multi(Sound::BUTTON_SOUND);

						GameEntryData entry_data;
						entry_data.appearance = _character_list[slot_index]->get_appearance();
						entry_data.stats = _character_list[slot_index]->get_player_stats();
						entry_data.character_name = _character_names[slot_index];

						scene_manager.set_scene<MainGameScene>(entry_data);
						return;
					}
				}

				_last_clicked_slot = slot_index;
				_last_click_time = current_time;

				sound_player.play_effect_multi(Sound::BUTTON_SOUND);
				SelectSlot(slot_index);
			};
		}
	}

	// Create character info labels for each slot
	{
		auto slot_rect = sprites[SPRID_CHARACTERSELECTSCREEN]->get_frame_rectangle(SPR_CHARACTERSELECTSCREEN::SELECTED_CHARACTER);
		const float slot_width = static_cast<float>(slot_rect.width);
		const float slot_start_x = 103.0f;
		const float slot_start_y = 54.0f;
		const float slot_gap = 2.0f;

		// Label config helper
		auto make_label_config = [](const std::string& text) {
			UI::Label::Config cfg;
			cfg.text = text;
			cfg.font_index = FontFamily::Default;
			cfg.font_size = 11;
			cfg.color = raylib::WHITE;
			cfg.font_style = FontStyle::Shadow;
			cfg.h_align = HorizontalAlign::Right;
			cfg.v_align = VerticalAlign::Middle;
			cfg.auto_size = false;
			return cfg;
		};

		for (size_t i = 0; i < MAX_CHARACTER_SLOTS; ++i) {
			float slot_x = slot_start_x + static_cast<float>(i) * (slot_width + slot_gap);

			// Name label: relative (9, 125, 88, 14)
			{
				auto& label = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()).get());
				_name_labels[i] = &label;
				label.SetBounds(slot_x + 9.0f, slot_start_y + 125.0f, 88.0f, 14.0f);

				// Set text if character exists in this slot
				if (i < _character_names.size()) {
					auto cfg = make_label_config(_character_names[i]);
					cfg.h_align = HorizontalAlign::Center;
					label.SetConfig(cfg);
				}
			}

			// Level label: relative (35, 142, 62, 11)
			{
				auto& label = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()).get());
				_level_labels[i] = &label;
				label.SetBounds(slot_x + 35.0f, slot_start_y + 142.0f, 62.0f, 11.0f);

				// Set text if character exists in this slot
				if (i < _character_list.size()) {
					auto cfg = make_label_config(std::to_string(_character_list[i]->get_player_stats().get_stat(StatType::LEVEL)));
					label.SetConfig(cfg);
				}
			}

			// Experience label: relative (35, 157, 62, 11)
			{
				auto& label = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()).get());
				_exp_labels[i] = &label;
				label.SetBounds(slot_x + 35.0f, slot_start_y + 157.0f, 62.0f, 11.0f);

				// Set text if character exists in this slot
				if (i < _character_list.size()) {
					auto cfg = make_label_config(std::to_string(_character_list[i]->get_player_stats().get_stat(StatType::EXPERIENCE)));
					label.SetConfig(cfg);
				}
			}
		}
	}

	// Helper lambda to create text config for extra large buttons
	auto make_text_config = [](const std::string& text) {
		UI::Button::TextConfig cfg;
		cfg.text = text;
		cfg.font_index = FontFamily::Fancy;
		cfg.font_size = 18;
		cfg.normal_color = rlx::RGB(230, 220, 200);
		cfg.hover_color = rlx::RGB(250, 225, 15);
		cfg.pressed_color = rlx::RGB(230, 220, 200);
		cfg.font_style = FontStyle::Shadow | FontStyle::Bold;
		cfg.h_align = HorizontalAlign::Center;
		cfg.v_align = VerticalAlign::Middle;
		cfg.offset_x = 0.0f;
		cfg.offset_y = 2.0f;
		return cfg;
	};

	// Helper lambda to create extra large button sprite config
	auto make_extra_large_button_sprite_config = []() {
		UI::Button::SpriteConfig cfg;
		cfg.sprite_id = SPRID_BUTTONS;
		cfg.normal_frame = SPR_BUTTONS::EXTRA_LARGE_TALL_BUTTON;
		cfg.hover_frame = SPR_BUTTONS::EXTRA_LARGE_TALL_BUTTON_HOVER;
		cfg.pressed_frame = SPR_BUTTONS::EXTRA_LARGE_TALL_BUTTON;
		cfg.disabled_frame = SPR_BUTTONS::EXTRA_LARGE_TALL_BUTTON;
		return cfg;
	};

	// Get button dimensions from sprite
	auto btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::EXTRA_LARGE_BUTTON);
	const float button_width = static_cast<float>(btn_rect.width);
	const float button_height = static_cast<float>(btn_rect.height);

	// Enter Game button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(364.0f, 288.0f, button_width, button_height);
		btn.SetSprite(make_extra_large_button_sprite_config());
		btn.SetText(make_text_config("Enter Game"));
		btn.OnClick = [this](UI::Control*) {
			// Check if a valid character is selected
			if (_selected_slot < 0 || static_cast<size_t>(_selected_slot) >= _character_list.size()) {
				return; // No valid character selected
			}

			sound_player.play_effect_multi(Sound::BUTTON_SOUND);

			// Create game entry data from selected character
			GameEntryData entry_data;
			entry_data.appearance = _character_list[_selected_slot]->get_appearance();
			entry_data.stats = _character_list[_selected_slot]->get_player_stats();
			entry_data.character_name = _character_names[_selected_slot];

			// Enter the game
			scene_manager.set_scene<MainGameScene>(entry_data);
		};
	}

	// Create New Character button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(364.0f, 318.0f, button_width, button_height);
		btn.SetSprite(make_extra_large_button_sprite_config());
		btn.SetText(make_text_config("Create New Character"));
		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			// Only allow creating new character if less than 4 characters exist
			if (_character_list.size() < 4) {
				scene_manager.set_scene<CreateCharacterScene>();
			}
		};
	}

	// Delete Character button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(364.0f, 348.0f, button_width, button_height);
		btn.SetSprite(make_extra_large_button_sprite_config());
		btn.SetText(make_text_config("Delete Character"));
		btn.OnClick = [this](UI::Control*) {
			// Check if a valid character is selected
			if (_selected_slot < 0 || static_cast<size_t>(_selected_slot) >= _character_list.size()) {
				return; // No valid character selected
			}

			sound_player.play_effect_multi(Sound::BUTTON_SOUND);

			// Get the character name for the confirmation dialog
			const std::string& character_name = _character_names[_selected_slot];
			int slot_to_delete = _selected_slot;

			// Show confirmation dialog
			scene_manager.push_overlay<MessageBoxDialog>(
				"Delete Character?",
				"Delete " + character_name + "?",
				MessageBoxDialog::ButtonType::YesNo
			);

			// Get the overlay and set the result callback
			// Note: This captures slot_to_delete and character_name by value
			auto* dialog = static_cast<MessageBoxDialog*>(scene_manager.get_current_overlay());
			if (dialog) {
				dialog->SetResultCallback([this, slot_to_delete, character_name](int result) {
					if (result == MessageBoxDialog::RESULT_YES) {
						// Build the save file path
						std::filesystem::path save_path = constant::SAVE_GAME_PATH / (character_name + ".jsave");

						// Delete the save file
						if (std::filesystem::exists(save_path)) {
							std::filesystem::remove(save_path);

							// Reload the scene to refresh the character list
							scene_manager.set_scene<CharacterSelectionScene>();
						}
					}
				});
			}
		};
	}

	// Change Password button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(364.0f, 378.0f, button_width, button_height);
		btn.SetSprite(make_extra_large_button_sprite_config());
		btn.SetText(make_text_config("Change Password"));
		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			// TODO: Change password logic
		};
	}

	// Logout button
	{
		auto& btn = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		btn.SetBounds(364.0f, 408.0f, button_width, button_height);
		btn.SetSprite(make_extra_large_button_sprite_config());
		btn.SetText(make_text_config("Logout"));
		btn.OnClick = [this](UI::Control*) {
			sound_player.play_effect_multi(Sound::BUTTON_SOUND);
			scene_manager.set_scene<MainMenuScene>();
		};
	}
}

void CharacterSelectionScene::on_uninitialize()
{
	// Clear static entity manager first (it holds raw pointers to players)
	static_entity_manager.Clear();

	// Clear the character list (destroys Player objects)
	_character_list.clear();
	_character_names.clear();

	// Clear UI controls
	m_controls.clear();

	// Reset slot button pointers
	_slot_buttons.fill(nullptr);

	// Reset label pointers
	_name_labels.fill(nullptr);
	_level_labels.fill(nullptr);
	_exp_labels.fill(nullptr);

	// Reset selection state
	_selected_slot = -1;
	_last_clicked_slot = -1;
	_last_click_time = 0.0f;
}

void CharacterSelectionScene::on_update()
{
	static_entity_manager.Update();

	for (auto& control : m_controls)
		control->Update();
}

void CharacterSelectionScene::on_render()
{
	sprites[SPRID_CHARACTERSELECTSCREEN]->draw(0, 0, SPR_CHARACTERSELECTSCREEN::BACKGROUND);

	static_entity_manager.Render();

	for (auto& control : m_controls)
		control->Render();
}

void CharacterSelectionScene::SelectSlot(int slot_index)
{
	// Deselect all slots first (radio button behavior)
	for (size_t i = 0; i < MAX_CHARACTER_SLOTS; ++i) {
		if (_slot_buttons[i]) {
			_slot_buttons[i]->SetToggled(false);
		}
	}

	// Select the clicked slot
	if (slot_index >= 0 && static_cast<size_t>(slot_index) < MAX_CHARACTER_SLOTS) {
		if (_slot_buttons[slot_index]) {
			_slot_buttons[slot_index]->SetToggled(true);
		}
		_selected_slot = slot_index;
	}
}