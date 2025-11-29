#pragma once
#include "Scene.h"

class CreateCharacterScene : public Scene {
public:
	PlayerAppearance created_appearance;
	Player created_character{ PlayerAppearance() };

public:
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;
};