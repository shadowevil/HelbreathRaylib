#pragma once
#include "Scene.h"
#include "player.h"

class CharacterSelectionScene : public Scene {
public:
	SCENE_TYPE(CharacterSelectionScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	std::vector<Player> _character_list{};
};