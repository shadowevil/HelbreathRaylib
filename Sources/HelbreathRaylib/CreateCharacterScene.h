#pragma once
#include "Scene.h"
#include "UI/Button.h"
#include "UI/ToggleButton.h"
#include "UI/Label.h"
#include "UI/InputBox.h"
#include <memory>
#include <vector>

enum SelectedClass : int8_t {
	CLASS_NONE = -1,
	CLASS_WARRIOR,
	CLASS_MAGICIAN,
	CLASS_MASTER
};

class CreateCharacterScene : public Scene {
public:
	SCENE_TYPE(CreateCharacterScene)

	PlayerAppearance created_appearance;
	Player created_character{ PlayerAppearance(), PlayerEntityStats()};
	PlayerEntityStats created_stats{};

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	void update_stat_label(UI::Label* label, StatType stat_type, int stat_value);
	void create_character_click();

private:
	SelectedClass selected_class = CLASS_NONE;

	// UI Controls
	std::vector<UI::ToggleButton*> class_buttons{};
	std::vector<std::unique_ptr<UI::Control>> m_controls{};

	bool stats_modified = true;

	UI::Label* stat_points_remaining_label = nullptr;
	UI::Label* stat_strength_label = nullptr;
	UI::Label* stat_vitality_label = nullptr;
	UI::Label* stat_dexterity_label = nullptr;
	UI::Label* stat_intelligence_label = nullptr;
	UI::Label* stat_magic_label = nullptr;
	UI::Label* stat_charisma_label = nullptr;

	UI::Label* derived_stat_health_label = nullptr;
	UI::Label* derived_stat_mana_label = nullptr;
	UI::Label* derived_stat_stamina_label = nullptr;

	UI::Button* create_button = nullptr;
	UI::InputBox* character_name_input = nullptr;

	UI::Label* top_left_label = nullptr;
	UI::Label* middle_left_label = nullptr;
	UI::Label* bottom_left_label = nullptr;
	UI::Label* bottom_right_label = nullptr;
};