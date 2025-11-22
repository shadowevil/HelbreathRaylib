#pragma once
#include <vector>

#include "Scene.h"

class MainMenuScene : public core::Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

private:
	enum MainMenuButton {
		LOGIN,
		NEW_ACCOUNT,
		EXIT,
		COUNT
	};
	std::vector<rlx::Rectangle<int16_t>> m_mainMenuButtonRects{};
};