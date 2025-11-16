#pragma once
#include "Scene.h"

class LoadingScene : public core::Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

private:
	uint8_t m_loadingStep = 0;
};
