#pragma once
#include "Scene.h"
#include "player.h"
#include "UI/Button.h"
#include "UI/ToggleButton.h"
#include "UI/Label.h"
#include <memory>
#include <vector>
#include <array>

class CharacterSelectionScene : public Scene {
public:
	SCENE_TYPE(CharacterSelectionScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	static constexpr size_t MAX_CHARACTER_SLOTS = 4;

	std::vector<std::unique_ptr<Player>> _character_list{};
	std::vector<std::string> _character_names{};
	std::vector<std::unique_ptr<UI::Control>> m_controls{};

	// Character slot selection
	std::array<UI::ToggleButton*, MAX_CHARACTER_SLOTS> _slot_buttons{};
	int _selected_slot = -1;  // -1 = no selection

	// Character info labels (per slot)
	std::array<UI::Label*, MAX_CHARACTER_SLOTS> _name_labels{};
	std::array<UI::Label*, MAX_CHARACTER_SLOTS> _level_labels{};
	std::array<UI::Label*, MAX_CHARACTER_SLOTS> _exp_labels{};

	// Double-click detection
	int _last_clicked_slot = -1;
	float _last_click_time = 0.0f;
	static constexpr float DOUBLE_CLICK_TIME = 0.4f;

	void SelectSlot(int slot_index);
};