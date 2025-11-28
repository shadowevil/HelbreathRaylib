#pragma once
#include "Scene.h"

class CreateCharacterScene : public Scene {
public:
	PlayerAppearance* created_appearance = nullptr;
	Player created_character{ PlayerAppearance() };

public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;
};