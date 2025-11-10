#pragma once
#include "raylib_include.h"
#include "../core/Scene.h"
#include "../core/Event.h"

// Pause Scene - Pause menu overlay
class PauseScene : public core::Scene {
public:
	PauseScene();
	~PauseScene() override = default;

	void Update(float deltaTime) override;
	void Render() override;
	void OnEvent(core::Event& event) override;

protected:
	void OnEnter() override;
	void OnExit() override;

private:
	int32_t m_selectedOption;
};
