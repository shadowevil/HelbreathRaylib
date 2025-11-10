#pragma once
#include "Scene.h"
#include "Application.h"
#include "raylib_include.h"
#include <string>

class LoadingScene : public core::Scene {
public:
	LoadingScene(core::Application& app) : core::Scene(app) {}
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;
};
