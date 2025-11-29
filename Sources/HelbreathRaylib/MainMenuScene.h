#pragma once
#include <vector>

#include "Scene.h"

class MainMenuScene : public Scene {
public:
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	enum MainMenuButton {
		LOGIN,
		NEW_ACCOUNT,
		EXIT,
		COUNT
	};
	std::vector<rlx::Rectangle<int16_t>> _main_menu_button_rects{};
};