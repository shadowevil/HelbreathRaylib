#pragma once
#include "raylib_include.h"
#include "core/Layer.h"
#include "core/Event.h"

// Main game layer for Helbreath
class Game : public core::Layer {
public:
	Game();
	~Game() override = default;

	void Initialize() override;
	void Uninitialize() override;
	void Update(float deltaTime) override;
	void Render() override;
	void OnEvent(core::Event& event) override;

private:
	// Game state and resources will go here
};
