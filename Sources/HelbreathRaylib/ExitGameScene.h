#pragma once
#include "Scene.h"

class ExitGameScene : public Scene {
public:
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;
};