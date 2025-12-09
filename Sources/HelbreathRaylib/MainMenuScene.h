#pragma once
#include "Scene.h"
#include "UI/Button.h"
#include <memory>
#include <vector>

class MainMenuScene : public Scene {
public:
	SCENE_TYPE(MainMenuScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::vector<std::unique_ptr<UI::Control>> m_controls{};
};