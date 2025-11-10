#pragma once
#include "raylib_include.h"
#include "../core/Scene.h"
#include "../core/Event.h"

// Menu Scene - Main menu for the game
class MenuScene : public core::Scene {
public:
	MenuScene();
	~MenuScene() override = default;

	void Update(float deltaTime) override;
	void Render() override;
	void OnEvent(core::Event& event) override;

protected:
	void OnEnter() override;
	void OnExit() override;

private:
	int32_t m_selectedOption;
	float m_animationTime;
};
