#include "Scenes.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

void CreateCharacterScene::on_initialize()
{
	// Configure static entity behavior
	StaticEntityManager::EntityConfig config;
	config.animation_type = AnimationType::WALK;
	config.weapon_used = WeaponUsed::HAND;
	config.direction_change_min_interval = 5.0;
	config.direction_change_max_interval = 10.0;
	config.randomize_direction = true;

	using namespace position_literals;
	static_entity_manager.AddPlayer(&created_character, GamePosition(500_p, 168_p), std::nullopt, config);
	created_stats = created_character.get_player_stats();
	created_stats.set_initial_stat_points(STARTING_STAT_POINTS);

	// Helper lambda to create text config
	auto make_text_config = [](const std::string& text) {
		UI::Button::TextConfig cfg;
		cfg.text = text;
		cfg.font_index = FontFamily::Default;
		cfg.font_size = 13;
		cfg.normal_color = rlx::RGB(230, 220, 200);
		cfg.hover_color = rlx::RGB(250, 225, 15);
		cfg.pressed_color = cfg.normal_color;
		cfg.font_style = FontStyle::Bold;
		cfg.h_align = HorizontalAlign::Center;
		cfg.v_align = VerticalAlign::Middle;
		return cfg;
		};

	// Helper lambda to create small button sprite config
	auto make_small_button_sprite_config = []() {
		UI::Button::SpriteConfig cfg;
		cfg.sprite_id = SPRID_BUTTONS;
		cfg.normal_frame = SPR_BUTTONS::SMALL_BUTTON;
		cfg.hover_frame = SPR_BUTTONS::SMALL_BUTTON_HOVER;
		return cfg;
		};

	// Helper lambda to create arrow button (only shows on hover)
	auto make_arrow_button = [this](float x, float y, int32_t hover_frame, std::function<void()> callback) -> UI::Button& {
		auto btn = static_cast<UI::Button*>(m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		auto rect = sprites[SPRID_CREATECHARARCTERSCREEN]->get_frame_rectangle(hover_frame);
		btn->SetBounds(x, y, static_cast<float>(rect.width), static_cast<float>(rect.height));

		UI::Button::SpriteConfig cfg;
		cfg.sprite_id = SPRID_CREATECHARARCTERSCREEN;
		cfg.hover_frame = hover_frame;
		btn->SetSprite(cfg);

		btn->OnClick = [callback](UI::Control*) { callback(); };
		return *btn;
		};

	// Helper lambda to create stat button pair
	auto make_stat_button = [this](float x, float y, int32_t hover_frame, std::function<void()> callback) -> UI::Button& {
		auto btn = static_cast<UI::Button*>(m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
		auto rect = sprites[SPRID_CREATECHARARCTERSCREEN]->get_frame_rectangle(hover_frame);
		btn->SetBounds(x, y, static_cast<float>(rect.width), static_cast<float>(rect.height));

		UI::Button::SpriteConfig cfg;
		cfg.sprite_id = SPRID_CREATECHARARCTERSCREEN;
		cfg.hover_frame = hover_frame;
		btn->SetSprite(cfg);

		btn->OnClick = [callback](UI::Control*) { callback(); };
		return *btn;
		};

	// Initialize class selection toggle buttons
	const char* class_names[] = { "Warrior", "Magician", "Master" };
	float class_x = 60.0f;
	auto small_btn_rect = sprites[SPRID_BUTTONS]->get_frame_rectangle(SPR_BUTTONS::SMALL_BUTTON);

	for (int i = 0; i < 3; i++) {
		auto btn = static_cast<UI::ToggleButton*>(m_controls.emplace_back(std::make_unique<UI::ToggleButton>(sprites)).get());
		btn->SetBounds(class_x, 445.0f, static_cast<float>(small_btn_rect.width), static_cast<float>(small_btn_rect.height));
		btn->SetSprite(make_small_button_sprite_config());
		btn->SetText(make_text_config(class_names[i]));

		int class_idx = i;
		btn->OnToggleChanged = [this, class_idx](UI::ToggleButton* toggled_btn, bool is_toggled) {
			if (is_toggled) {
				for (auto& other_btn : class_buttons) {
					if (other_btn != toggled_btn) {
						other_btn->SetToggled(false);
					}
				}
				sound_player.play_effect_multi(Sound::BUTTON_SOUND);
				selected_class = static_cast<SelectedClass>(class_idx);
			}
			else {
				selected_class = CLASS_NONE;
			}
			};
		btn->OnMouseHover = [this, class_idx](UI::Control* toggle_btn) {
			if (static_cast<SelectedClass>(class_idx) == CLASS_WARRIOR)
			{
				bottom_left_label->SetText("Warrior: Balanced stats with a focus on strength and defense.");
			}
			else if(static_cast<SelectedClass>(class_idx) == CLASS_MAGICIAN)
			{
				bottom_left_label->SetText("Magician: High intelligence and mana, but lower physical stats.");
			}
			else if (static_cast<SelectedClass>(class_idx) == CLASS_MASTER)
			{
				bottom_left_label->SetText("Master: Typically high charisma for handling guild operations.");
			}
			else {
				bottom_left_label->SetText("You can manually apply your stats or use one of the preset stat options below.");
			}
			};
		btn->OnMouseLeave = [this](UI::Control*) {
			if(selected_class == CLASS_NONE)
				bottom_left_label->SetText("You can manually apply your stats or use one of the preset stat options below.");

			if (selected_class == CLASS_WARRIOR)
				bottom_left_label->SetText("Warrior: Balanced stats with a focus on strength and defense.");
			else if (selected_class == CLASS_MAGICIAN)
				bottom_left_label->SetText("Magician: High intelligence and mana, but lower physical stats.");
			else if (selected_class == CLASS_MASTER)
				bottom_left_label->SetText("Master: Typically high charisma for handling guild operations.");
			};

		class_x += static_cast<float>(small_btn_rect.width) + 10.0f;
		class_buttons.push_back(btn);
	}

	// Create and Cancel buttons
	create_button = dynamic_cast<UI::Button*>(m_controls.emplace_back(std::make_unique<UI::Button>(sprites)).get());
	create_button->SetBounds(385.0f, 445.0f, static_cast<float>(small_btn_rect.width), static_cast<float>(small_btn_rect.height));
	create_button->SetSprite(make_small_button_sprite_config());
	create_button->SetText(make_text_config("Create"));
	create_button->SetEnabled(false);
	create_button->OnClick = [this](UI::Control*) {
		create_character_click();
		};
	create_button->OnMouseHover = [this](UI::Control*) {
		bottom_right_label->SetText("Create your character with the selected stat points and appearance values.");
		};
	create_button->OnMouseLeave = [this](UI::Control*) {
		bottom_right_label->SetText("Welcome to helbreath! You can create your character by selecting a class, which will allocate points for you. You may also manually set points as you wish.");
		};

	UI::Button& cancel_button = static_cast<UI::Button&>(*m_controls.emplace_back(std::make_unique<UI::Button>(sprites)));
	cancel_button.SetBounds(385.0f + static_cast<float>(small_btn_rect.width) + 40.0f, 445.0f, static_cast<float>(small_btn_rect.width), static_cast<float>(small_btn_rect.height));
	cancel_button.SetSprite(make_small_button_sprite_config());
	cancel_button.SetText(make_text_config("Cancel"));
	cancel_button.OnClick = [this](UI::Control*) {
		// TODO: Cancel logic
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		scene_manager.set_scene<CharacterSelectionScene>();
		};
	cancel_button.OnMouseHover = [this](UI::Control*) {
		bottom_right_label->SetText("Navigate back to the select character screen. If you don't have any characters created you will be redirected back to the main menu screen.");
		};
	cancel_button.OnMouseLeave = [this](UI::Control*) {
		bottom_right_label->SetText("Welcome to helbreath! You can create your character by selecting a class, which will allocate points for you. You may also manually set points as you wish.");
		};

	// Appearance controls - using helper functions to reduce duplication
	auto arrow_left_rect = sprites[SPRID_CREATECHARARCTERSCREEN]->get_frame_rectangle(SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT);
	auto arrow_right_rect = sprites[SPRID_CREATECHARARCTERSCREEN]->get_frame_rectangle(SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT);
	float arrow_spacing = static_cast<float>(arrow_left_rect.width) + 1.0f;
	float row_height = static_cast<float>(arrow_left_rect.height) + 2.0f;

	// Gender
	float current_y = 157.0f;
	UI::Button& gender_decrease = make_arrow_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.gender = (appearance.gender == GENDER_MALE) ? GENDER_FEMALE : GENDER_MALE;
		created_character.update_appearance(appearance);
		});
	UI::Button& gender_increase = make_arrow_button(236.0f + arrow_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.gender = (appearance.gender == GENDER_MALE) ? GENDER_FEMALE : GENDER_MALE;
		created_character.update_appearance(appearance);
		});
	gender_decrease.OnMouseHover = gender_increase.OnMouseHover = gender_increase.OnMouseUp = gender_decrease.OnMouseUp = [this](UI::Control*) {
		auto appearance = created_character.get_appearance();
		std::string next_gender = (appearance.gender == GENDER_MALE ?
			"Change gender to Female" :
			"Change gender to Male");
		top_left_label->SetText(next_gender);
		};
	gender_decrease.OnMouseLeave = gender_increase.OnMouseLeave = [this](UI::Control*) {
		top_left_label->SetText("Change your appearance!");
		};

	// Skin Color
	current_y += row_height;
	UI::Button& skin_color_decrease = make_arrow_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.skin_color_index = (appearance.skin_color_index + SkinColorCount - 1) % SkinColorCount;
		created_character.update_appearance(appearance);
		});
	skin_color_decrease.OnMouseHover = [this](UI::Control*) {
		std::string previous_skin_color = "Previous skin color";
		top_left_label->SetText(previous_skin_color);
		};
	UI::Button& skin_color_increase = make_arrow_button(236.0f + arrow_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.skin_color_index = (appearance.skin_color_index + 1) % SkinColorCount;
		created_character.update_appearance(appearance);
		});
	skin_color_increase.OnMouseHover = [this](UI::Control*) {
		std::string next_skin_color = "Next skin color";
		top_left_label->SetText(next_skin_color);
		};

	skin_color_decrease.OnMouseLeave = skin_color_increase.OnMouseLeave = [this](UI::Control*) {
		top_left_label->SetText("Change your appearance!");
		};

	// Underwear Color
	current_y += row_height;
	UI::Button& underwear_color_decrease = make_arrow_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.underwear_color_index = (appearance.underwear_color_index + UnderwearColorCount - 1) % UnderwearColorCount;
		created_character.update_appearance(appearance);
		});
	underwear_color_decrease.OnMouseHover = [this](UI::Control*) {
		std::string previous_underwear_color = "Previous underwear color";
		top_left_label->SetText(previous_underwear_color);
		};

	UI::Button& underwear_color_increase = make_arrow_button(236.0f + arrow_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.underwear_color_index = (appearance.underwear_color_index + 1) % UnderwearColorCount;
		created_character.update_appearance(appearance);
		});
	underwear_color_increase.OnMouseHover = [this](UI::Control*) {
		std::string next_underwear_color = "Next underwear color";
		top_left_label->SetText(next_underwear_color);
		};

	underwear_color_decrease.OnMouseLeave = underwear_color_increase.OnMouseLeave = [this](UI::Control*) {
		top_left_label->SetText("Change your appearance!");
		};

	// Hair Style
	current_y += row_height;
	UI::Button& hairstyle_decrease = make_arrow_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		int idx = (appearance.hair_style + 1);
		idx = (idx - 1 + (HAIR_STYLE_COUNT + 1)) % (HAIR_STYLE_COUNT + 1);
		appearance.hair_style = HairStyle(idx - 1);
		created_character.update_appearance(appearance);
		});
	hairstyle_decrease.OnMouseHover = [this](UI::Control*) {
		std::string previous_hairstyle = "Previous hairstyle";
		top_left_label->SetText(previous_hairstyle);
		};

	UI::Button& hairstyle_increase = make_arrow_button(236.0f + arrow_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		int idx = (appearance.hair_style + 1);
		idx = (idx + 1) % (HAIR_STYLE_COUNT + 1);
		appearance.hair_style = HairStyle(idx - 1);
		created_character.update_appearance(appearance);
		});
	hairstyle_increase.OnMouseHover = [this](UI::Control*) {
		std::string next_hairstyle = "Next hairstyle";
		top_left_label->SetText(next_hairstyle);
		};

	hairstyle_decrease.OnMouseLeave = hairstyle_increase.OnMouseLeave = [this](UI::Control*) {
		top_left_label->SetText("Change your appearance!");
		};

	// Hair Color
	current_y += row_height;
	UI::Button& haircolor_decrease = make_arrow_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_LEFT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.hair_color_index = (appearance.hair_color_index + HairColorCount - 1) % HairColorCount;
		created_character.update_appearance(appearance);
		});
	haircolor_decrease.OnMouseHover = [this](UI::Control*) {
		std::string previous_hair_color = "Previous hair color";
		top_left_label->SetText(previous_hair_color);
		};

	UI::Button& haircolor_increase = make_arrow_button(236.0f + arrow_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_ARROW_RIGHT, [this]() {
		sound_player.play_effect_multi(Sound::BUTTON_SOUND);
		auto appearance = created_character.get_appearance();
		appearance.hair_color_index = (appearance.hair_color_index + 1) % HairColorCount;
		created_character.update_appearance(appearance);
		});
	haircolor_increase.OnMouseHover = [this](UI::Control*) {
		std::string next_hair_color = "Next hair color";
		top_left_label->SetText(next_hair_color);
		};

	haircolor_decrease.OnMouseLeave = haircolor_increase.OnMouseLeave = [this](UI::Control*) {
		top_left_label->SetText("Change your appearance!");
		};


	// Stat controls
	auto plus_rect = sprites[SPRID_CREATECHARARCTERSCREEN]->get_frame_rectangle(SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS);
	float stat_spacing = static_cast<float>(plus_rect.width) + 1.0f;
	float stat_row_height = static_cast<float>(plus_rect.height) + 2.0f;

	// Strength
	current_y = 276.0f;
	UI::Button& stat_strength_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_strength_label, StatType::STRENGTH, 1);
		});
	UI::Button& stat_strength_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_strength_label, StatType::STRENGTH, -1);
		});

	stat_strength_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase strength by 1");
		};
	stat_strength_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease strength by 1");
		};
	stat_strength_increase.OnMouseLeave = stat_strength_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Vitality
	current_y += stat_row_height;
	UI::Button& stat_vitality_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_vitality_label, StatType::VITALITY, 1);
		});
	UI::Button& stat_vitality_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_vitality_label, StatType::VITALITY, -1);
		});

	stat_vitality_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase vitality by 1");
		};
	stat_vitality_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease vitality by 1");
		};
	stat_vitality_increase.OnMouseLeave = stat_vitality_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Dexterity
	current_y += stat_row_height;
	UI::Button& stat_dexterity_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_dexterity_label, StatType::DEXTERITY, 1);
		});
	UI::Button& stat_dexterity_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_dexterity_label, StatType::DEXTERITY, -1);
		});

	stat_dexterity_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase dexterity by 1");
		};
	stat_dexterity_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease dexterity by 1");
		};
	stat_dexterity_increase.OnMouseLeave = stat_dexterity_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Intelligence
	current_y += stat_row_height;
	UI::Button& stat_intelligence_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_intelligence_label, StatType::INTELLIGENCE, 1);
		});
	UI::Button& stat_intelligence_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_intelligence_label, StatType::INTELLIGENCE, -1);
		});

	stat_intelligence_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase intelligence by 1");
		};
	stat_intelligence_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease intelligence by 1");
		};
	stat_intelligence_increase.OnMouseLeave = stat_intelligence_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Magic
	current_y += stat_row_height;
	UI::Button& stat_magic_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_magic_label, StatType::MAGIC, 1);
		});
	UI::Button& stat_magic_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_magic_label, StatType::MAGIC, -1);
		});

	stat_magic_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase magic by 1");
		};
	stat_magic_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease magic by 1");
		};
	stat_magic_increase.OnMouseLeave = stat_magic_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Charisma
	current_y += stat_row_height;
	UI::Button& stat_charisma_increase = make_stat_button(236.0f, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_PLUS, [this]() {
		update_stat_label(stat_charisma_label, StatType::CHARISMA, 1);
		});
	UI::Button& stat_charisma_decrease = make_stat_button(236.0f + stat_spacing, current_y, SPR_CREATECHARARCTERSCREEN::BUTTON_MINUS, [this]() {
		update_stat_label(stat_charisma_label, StatType::CHARISMA, -1);
		});

	stat_charisma_increase.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Increase charisma by 1");
		};
	stat_charisma_decrease.OnMouseHover = [this](UI::Control*) {
		middle_left_label->SetText("Decrease charisma by 1");
		};
	stat_charisma_increase.OnMouseLeave = stat_charisma_decrease.OnMouseLeave = [this](UI::Control*) {
		middle_left_label->SetText("Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.");
		};

	// Initialize labels
	UI::Label::Config label_config;
	label_config.font_index = FontFamily::Default;
	label_config.font_size = 13;
	label_config.color = rlx::RGB(33, 33, 33);
	label_config.h_align = HorizontalAlign::Center;
	label_config.v_align = VerticalAlign::Middle;
	label_config.auto_size = false;
	label_config.word_wrap = true;
	label_config.break_mode = UI::TextBreakMode::Word;
	label_config.use_hyphenation = false;

	// Character name instruction label
	UI::Label& character_name_label = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	character_name_label.SetBounds(60.0f, 90.0f, 241.0f, 15.0f);
	character_name_label.SetLineSpacing(2.0f);
	character_name_label.SetOverflowHorizontal(UI::TextOverflow::Ellipsis);
	character_name_label.SetOverflowVertical(UI::TextOverflow::Ellipsis);
	label_config.text = "Enter your desired character name";
	character_name_label.SetConfig(label_config);

	// Top left dynamic display text
	top_left_label = static_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	top_left_label->SetBounds(60.0f, 126.0f, 241.0f, 30.0f);
	top_left_label->SetLineSpacing(2.0f);
	top_left_label->SetOverflowHorizontal(UI::TextOverflow::Ellipsis);
	top_left_label->SetOverflowVertical(UI::TextOverflow::Ellipsis);
	label_config.text = "Change your appearance!";
	top_left_label->SetConfig(label_config);

	// Middle left dynamic display text
	middle_left_label = static_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	middle_left_label->SetBounds(60.0f, 241.0f, 241.0f, 30.0f);
	middle_left_label->SetLineSpacing(2.0f);
	middle_left_label->SetOverflowHorizontal(UI::TextOverflow::Ellipsis);
	middle_left_label->SetOverflowVertical(UI::TextOverflow::Ellipsis);
	label_config.text = "Change your stats. You may have a maximum of 14 and a minimum of 10 per stat.";
	middle_left_label->SetConfig(label_config);

	// Bottom left dynamic display text
	bottom_left_label = static_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	bottom_left_label->SetBounds(60.0f, 375.0f, 241.0f, 60.0f);
	bottom_left_label->SetLineSpacing(2.0f);
	bottom_left_label->SetOverflowHorizontal(UI::TextOverflow::Ellipsis);
	bottom_left_label->SetOverflowVertical(UI::TextOverflow::Ellipsis);
	label_config.text = "You can manually apply your stats or use one of the preset stat options below.";
	bottom_left_label->SetConfig(label_config);

	// Bottom right dynamic display text
	bottom_right_label = static_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	bottom_right_label->SetBounds(372.0f, 318.0f, 211.0f, 120.0f);
	bottom_right_label->SetLineSpacing(2.0f);
	bottom_right_label->SetOverflowHorizontal(UI::TextOverflow::Ellipsis);
	bottom_right_label->SetOverflowVertical(UI::TextOverflow::Ellipsis);
	label_config.text = "Welcome to helbreath! You can create your character by selecting a class, which will allocate points for you. You may also manually set points as you wish.";
	bottom_right_label->SetConfig(label_config);

	// Field labels (right-aligned, font size 14, black color)
	UI::Label::Config field_label_config;
	field_label_config.font_index = FontFamily::Default;
	field_label_config.font_size = 14;
	field_label_config.color = rlx::RGB(0, 0, 0);
	field_label_config.h_align = HorizontalAlign::Right;
	field_label_config.v_align = VerticalAlign::Middle;
	field_label_config.auto_size = false;

	UI::Label& label_character_name = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_character_name.SetBounds(60.0f, 106.0f, 130.0f, 21.0f);
	field_label_config.text = "Character Name: ";
	label_character_name.SetConfig(field_label_config);

	UI::Label& label_gender = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_gender.SetBounds(60.0f, 157.0f + 16.0f * 0, 173.0f, 14.0f);
	field_label_config.text = "Gender: ";
	label_gender.SetConfig(field_label_config);

	UI::Label& label_skin_color = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_skin_color.SetBounds(60.0f, 157.0f + 16.0f * 1, 173.0f, 14.0f);
	field_label_config.text = "Skin Color: ";
	label_skin_color.SetConfig(field_label_config);

	UI::Label& label_underwear_color = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_underwear_color.SetBounds(60.0f, 157.0f + 16.0f * 2, 173.0f, 14.0f);
	field_label_config.text = "Underwear Color: ";
	label_underwear_color.SetConfig(field_label_config);

	UI::Label& label_hair_style = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_hair_style.SetBounds(60.0f, 157.0f + 16.0f * 3, 173.0f, 14.0f);
	field_label_config.text = "Hair Style: ";
	label_hair_style.SetConfig(field_label_config);

	UI::Label& label_hair_color = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_hair_color.SetBounds(60.0f, 157.0f + 16.0f * 4, 173.0f, 14.0f);
	field_label_config.text = "Hair Color: ";
	label_hair_color.SetConfig(field_label_config);

	// Stat labels (right-aligned, font size 14, black color)
	UI::Label& label_strength = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_strength.SetBounds(60.0f, 275.0f + 16.0f * 0, 130.0f, 14.0f);
	field_label_config.text = "Strength: ";
	label_strength.SetConfig(field_label_config);

	// Strength stat value label
	stat_strength_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_strength_label->SetBounds(193.0f, 276.0f + 16.0f * 0.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::STRENGTH));
	stat_strength_label->SetConfig(field_label_config);
	stat_strength_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_vitality = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_vitality.SetBounds(60.0f, 275.0f + 16.0f * 1, 130.0f, 14.0f);
	field_label_config.text = "Vitality: ";
	label_vitality.SetConfig(field_label_config);

	// vitality stat value label
	stat_vitality_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_vitality_label->SetBounds(193.0f, 276.0f + 16.0f * 1.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::VITALITY));
	stat_vitality_label->SetConfig(field_label_config);
	stat_vitality_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_dexterity = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_dexterity.SetBounds(60.0f, 275.0f + 16.0f * 2, 130.0f, 14.0f);
	field_label_config.text = "Dexterity: ";
	label_dexterity.SetConfig(field_label_config);

	// dexterity stat value label
	stat_dexterity_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_dexterity_label->SetBounds(193.0f, 276.0f + 16.0f * 2.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::DEXTERITY));
	stat_dexterity_label->SetConfig(field_label_config);
	stat_dexterity_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_intelligence = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_intelligence.SetBounds(60.0f, 275.0f + 16.0f * 3, 130.0f, 14.0f);
	field_label_config.text = "Intelligence: ";
	label_intelligence.SetConfig(field_label_config);

	// intelligence stat value label
	stat_intelligence_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_intelligence_label->SetBounds(193.0f, 276.0f + 16.0f * 3.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::INTELLIGENCE));
	stat_intelligence_label->SetConfig(field_label_config);
	stat_intelligence_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_magic = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_magic.SetBounds(60.0f, 275.0f + 16.0f * 4, 130.0f, 14.0f);
	field_label_config.text = "Magic: ";
	label_magic.SetConfig(field_label_config);

	// magic stat value label
	stat_magic_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_magic_label->SetBounds(193.0f, 276.0f + 16.0f * 4.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::MAGIC));
	stat_magic_label->SetConfig(field_label_config);
	stat_magic_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_charisma = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_charisma.SetBounds(60.0f, 275.0f + 16.0f * 5, 130.0f, 14.0f);
	field_label_config.text = "Charisma: ";
	label_charisma.SetConfig(field_label_config);

	// charisma stat value label
	stat_charisma_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_charisma_label->SetBounds(193.0f, 276.0f + 16.0f * 5.0f, 36.0f, 14.0f);
	field_label_config.h_align = HorizontalAlign::Center;
	field_label_config.text = std::to_string(created_stats.get_stat(StatType::CHARISMA));
	stat_charisma_label->SetConfig(field_label_config);
	stat_charisma_label->SetColor(rlx::RGB(64, 64, 64));
	field_label_config.h_align = HorizontalAlign::Right;

	stat_points_remaining_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	stat_points_remaining_label->SetBounds(60.0f, 275.0f + 16.0f * 6, 241.0f, 15.0f);
	field_label_config.text = "Available stat points: " + std::to_string(created_stats.get_available_stat_points());
	field_label_config.h_align = HorizontalAlign::Center;
	stat_points_remaining_label->SetConfig(field_label_config);
	field_label_config.h_align = HorizontalAlign::Right;

	// Info labels (right-aligned, font size 14, black color)
	UI::Label& label_health = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_health.SetBounds(448.0f, 190.0f + 15.0f * 0, 90.0f, 15.0f);
	field_label_config.text = "Health: ";
	label_health.SetConfig(field_label_config);

	// Health stat value label
	derived_stat_health_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	derived_stat_health_label->SetBounds(540.0f, 191.0f + 16.0f * 0.0f, 36.0f, 14.0f);
	field_label_config.text = std::to_string(created_stats.get_max_stat(StatType::HEALTH));
	field_label_config.h_align = HorizontalAlign::Center;
	derived_stat_health_label->SetConfig(field_label_config);
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_mana = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_mana.SetBounds(448.0f, 190.0f + 15.0f * 1, 90.0f, 15.0f);
	field_label_config.text = "Mana: ";
	label_mana.SetConfig(field_label_config);

	// Mana stat value label
	derived_stat_mana_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	derived_stat_mana_label->SetBounds(540.0f, 191.0f + 16.0f * 1.0f, 36.0f, 14.0f);
	field_label_config.text = std::to_string(created_stats.get_max_stat(StatType::MANA));
	field_label_config.h_align = HorizontalAlign::Center;
	derived_stat_mana_label->SetConfig(field_label_config);
	field_label_config.h_align = HorizontalAlign::Right;

	UI::Label& label_stamina = static_cast<UI::Label&>(*m_controls.emplace_back(std::make_unique<UI::Label>()));
	label_stamina.SetBounds(448.0f, 190.0f + 15.0f * 2, 90.0f, 15.0f);
	field_label_config.text = "Stamina: ";
	label_stamina.SetConfig(field_label_config);

	// Stamina stat value label
	derived_stat_stamina_label = dynamic_cast<UI::Label*>(m_controls.emplace_back(std::make_unique<UI::Label>()).get());
	derived_stat_stamina_label->SetBounds(540.0f, 191.0f + 16.0f * 2.0f, 36.0f, 14.0f);
	field_label_config.text = std::to_string(created_stats.get_max_stat(StatType::STAMINA));
	field_label_config.h_align = HorizontalAlign::Center;
	derived_stat_stamina_label->SetConfig(field_label_config);
	field_label_config.h_align = HorizontalAlign::Right;

	UI::InputBox::Config input_box_config;
	input_box_config.font_index = FontFamily::Default;
	input_box_config.cursor_width = 1.0f;
	input_box_config.max_length = 10;
	input_box_config.text_color = raylib::Color{ 232, 232, 232, 255 };
	input_box_config.placeholder_text = "Character name...";

	character_name_input = dynamic_cast<UI::InputBox*>(m_controls.emplace_back(std::make_unique<UI::InputBox>()).get());
	character_name_input->SetBounds(193, 108, 106, 17);
	character_name_input->SetAllowedCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
	character_name_input->SetConfig(input_box_config);
}

void CreateCharacterScene::on_uninitialize()
{
	static_entity_manager.Remove(&created_character);
	class_buttons.clear();
	m_controls.clear();
}

void CreateCharacterScene::on_update()
{
	static_entity_manager.Update();

	for (auto& control : m_controls) {
		control->Update();
	}

	created_stats.calculate_all();

	if (stats_modified)
	{
		derived_stat_health_label->SetText(std::to_string(created_stats.get_max_stat(StatType::HEALTH)));
		derived_stat_mana_label->SetText(std::to_string(created_stats.get_max_stat(StatType::MANA)));
		derived_stat_stamina_label->SetText(std::to_string(created_stats.get_max_stat(StatType::STAMINA)));
		stat_points_remaining_label->SetText("Available stat points: " + std::to_string(created_stats.get_available_stat_points()));
		stats_modified = false;
	}

	if (selected_class != CLASS_NONE)
	{
		switch (selected_class)
		{
		case CLASS_WARRIOR:
			created_stats.set_initial_stat_points(0);
			created_stats.set_stat(StatType::STRENGTH, 14);
			created_stats.set_stat(StatType::VITALITY, 14);
			created_stats.set_stat(StatType::DEXTERITY, 12);
			created_stats.set_stat(StatType::INTELLIGENCE, 10);
			created_stats.set_stat(StatType::MAGIC, 10);
			created_stats.set_stat(StatType::CHARISMA, 10);
			break;
		case CLASS_MAGICIAN:
			created_stats.set_initial_stat_points(0);
			created_stats.set_stat(StatType::STRENGTH, 10);
			created_stats.set_stat(StatType::VITALITY, 12);
			created_stats.set_stat(StatType::DEXTERITY, 10);
			created_stats.set_stat(StatType::INTELLIGENCE, 14);
			created_stats.set_stat(StatType::MAGIC, 14);
			created_stats.set_stat(StatType::CHARISMA, 10);
			break;
		case CLASS_MASTER:
			created_stats.set_initial_stat_points(0);
			created_stats.set_stat(StatType::STRENGTH, 12);
			created_stats.set_stat(StatType::VITALITY, 12);
			created_stats.set_stat(StatType::DEXTERITY, 12);
			created_stats.set_stat(StatType::INTELLIGENCE, 10);
			created_stats.set_stat(StatType::MAGIC, 10);
			created_stats.set_stat(StatType::CHARISMA, 14);
			break;
		}
		stat_strength_label->SetText(std::to_string(created_stats.get_stat(StatType::STRENGTH)));
		stat_vitality_label->SetText(std::to_string(created_stats.get_stat(StatType::VITALITY)));
		stat_dexterity_label->SetText(std::to_string(created_stats.get_stat(StatType::DEXTERITY)));
		stat_intelligence_label->SetText(std::to_string(created_stats.get_stat(StatType::INTELLIGENCE)));
		stat_magic_label->SetText(std::to_string(created_stats.get_stat(StatType::MAGIC)));
		stat_charisma_label->SetText(std::to_string(created_stats.get_stat(StatType::CHARISMA)));
		derived_stat_health_label->SetText(std::to_string(created_stats.get_max_stat(StatType::HEALTH)));
		derived_stat_mana_label->SetText(std::to_string(created_stats.get_max_stat(StatType::MANA)));
		derived_stat_stamina_label->SetText(std::to_string(created_stats.get_max_stat(StatType::STAMINA)));
		stat_points_remaining_label->SetText("Available stat points: " + std::to_string(created_stats.get_available_stat_points()));

		raylib::Color unchanged_color = rlx::RGB(64, 64, 64);
		raylib::Color changed_color = rlx::RGB(0, 232, 0);
		if (created_stats.get_stat(StatType::STRENGTH) > 10)
		{
			stat_strength_label->SetColor(changed_color);
		}
		else {
			stat_strength_label->SetColor(unchanged_color);
		}

		if (created_stats.get_stat(StatType::VITALITY) > 10)
		{
			stat_vitality_label->SetColor(changed_color);
		}
		else {
			stat_vitality_label->SetColor(unchanged_color);
		}

		if (created_stats.get_stat(StatType::DEXTERITY) > 10)
		{
			stat_dexterity_label->SetColor(changed_color);
		}
		else {
			stat_dexterity_label->SetColor(unchanged_color);
		}

		if (created_stats.get_stat(StatType::INTELLIGENCE) > 10)
		{
			stat_intelligence_label->SetColor(changed_color);
		}
		else {
			stat_intelligence_label->SetColor(unchanged_color);
		}

		if (created_stats.get_stat(StatType::MAGIC) > 10)
		{
			stat_magic_label->SetColor(changed_color);
		}
		else {
			stat_magic_label->SetColor(unchanged_color);
		}

		if (created_stats.get_stat(StatType::CHARISMA) > 10)
		{
			stat_charisma_label->SetColor(changed_color);
		}
		else {
			stat_charisma_label->SetColor(unchanged_color);
		}
	}

	if (created_stats.get_available_stat_points() == 0 &&
		character_name_input->GetText().length() > 0)
	{
		create_button->SetEnabled(true);
	}
}

void CreateCharacterScene::on_render()
{
	sprites[SPRID_CREATECHARARCTERSCREEN]->draw(0, 0, SPR_CREATECHARARCTERSCREEN::BACKGROUND);

	for (auto& control : m_controls) {
		control->Render();
	}

	static_entity_manager.Render();
}

void CreateCharacterScene::update_stat_label(UI::Label* label, StatType stat_type, int stat_value)
{
	uint32_t current_value = created_stats.get_stat(stat_type);
	int new_value = static_cast<int>(current_value) + stat_value;
	if (new_value < 10 || new_value > 14) return;
	if (stat_value > 0 && created_stats.get_available_stat_points() < static_cast<uint32_t>(stat_value)) return;
	if (stat_value < 0 && current_value <= 10) return;

	created_stats.stat_add(stat_type, stat_value);
	label->SetText(std::to_string(created_stats.get_stat(stat_type)));
	sound_player.play_effect_multi(Sound::BUTTON_SOUND);
	stats_modified = true;

	raylib::Color changed_color = rlx::RGB(64, 64, 64);

	if (new_value < 10)
	{
		changed_color = rlx::RGB(128, 0, 0);
	}
	else if (new_value > 10)
	{
		changed_color = rlx::RGB(0, 232, 0);
	}

	label->SetColor(changed_color);
}

void CreateCharacterScene::create_character_click()
{
	// Ensure save directory exists
	if (!rlx::Directory::Exists(constant::SAVE_GAME_PATH)) {
		rlx::Directory::Create(constant::SAVE_GAME_PATH);
	}

	// Check if character name already exists
	std::filesystem::path save_path = constant::SAVE_GAME_PATH / (character_name_input->GetText() + ".jsave");
	if (std::filesystem::exists(save_path)) {
		sound_player.play_effect_multi(Sound::FAILURE);

		// Show error dialog
		scene_manager.push_overlay<MessageBoxDialog>(
			"Name Already Exists",
			"Please choose a different name.",
			MessageBoxDialog::ButtonType::Ok
		);
		return;
	}

	created_character.commit_new_player_stats(created_stats);

	// Create JSON object with character data
	json character_data;
	character_data["name"] = character_name_input->GetText();
	character_data["player"] = created_character;

	// Save to file in SAVEDGAMES directory with .jsave extension
	std::ofstream file(save_path);
	if (file.is_open()) {
		file << character_data.dump(4); // Pretty print with 4 space indentation
		file.close();

		// Success - play sound and show success dialog
		sound_player.play_effect_multi(Sound::SUCCESS_SOUND);

		scene_manager.push_overlay<MessageBoxDialog>(
			"Character Created!",
			character_name_input->GetText() + " has been created.",
			MessageBoxDialog::ButtonType::Ok
		);

		// Navigate to character selection when dialog is closed
		auto* dialog = static_cast<MessageBoxDialog*>(scene_manager.get_current_overlay());
		if (dialog) {
			dialog->SetResultCallback([this](int) {
				scene_manager.set_scene<CharacterSelectionScene>();
			});
		}
	}
	else {
		// Failed to save - show error dialog
		sound_player.play_effect_multi(Sound::FAILURE);

		scene_manager.push_overlay<MessageBoxDialog>(
			"Creation Failed",
			"Could not save character file.",
			MessageBoxDialog::ButtonType::Ok
		);
	}
}
