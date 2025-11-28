#pragma once
#include "Scene.h"

class LoadingScene : public Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

private:
	uint8_t m_loadingStep = 0;

	void LoadSprite(size_t uStart, size_t uCount, const std::string& file, CSpriteCollection& collection);
	void LoadScenes();
	void LoadInterface();
	void LoadMaleGameModels();
	void LoadFemaleGameModels();
	void LoadItems();
	void LoadRegisterMaps();
};
