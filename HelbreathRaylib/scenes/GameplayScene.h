#pragma once
#include "raylib_include.h"
#include "../core/Scene.h"
#include "../core/Event.h"

// Gameplay Scene - Main game scene
class GameplayScene : public core::Scene {
public:
	GameplayScene();
	~GameplayScene() override = default;

	void Update(float deltaTime) override;
	void Render() override;
	void OnEvent(core::Event& event) override;

protected:
	void OnEnter() override;
	void OnExit() override;

private:
	float m_gameTime;
	bool m_paused;
};
