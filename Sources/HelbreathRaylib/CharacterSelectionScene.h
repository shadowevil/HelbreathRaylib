#pragma once
#include "Scene.h"
#include "player.h"

class CharacterSelectionScene : public Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

private:
	std::vector<Player> m_characterList{};
};