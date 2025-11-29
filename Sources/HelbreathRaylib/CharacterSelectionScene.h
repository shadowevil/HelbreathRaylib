#pragma once
#include "Scene.h"
#include "player.h"

class CharacterSelectionScene : public Scene {
public:
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::vector<Player> _character_list{};
};