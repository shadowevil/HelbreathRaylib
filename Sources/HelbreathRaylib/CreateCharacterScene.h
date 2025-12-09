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
	Player created_character{ PlayerAppearance() };

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	SelectedClass selected_class = CLASS_NONE;

	// UI Controls
	std::vector<UI::ToggleButton*> class_buttons{};
	std::vector<std::unique_ptr<UI::Control>> m_controls{};

	UI::Label* top_left_label = nullptr;
	UI::Label* middle_left_label = nullptr;
	UI::Label* bottom_left_label = nullptr;
	UI::Label* bottom_right_label = nullptr;
};