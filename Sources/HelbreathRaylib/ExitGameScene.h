#pragma once
#include "Scene.h"

class ExitGameScene : public core::Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;
};