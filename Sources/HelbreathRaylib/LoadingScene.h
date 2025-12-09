#pragma once
#include "Scene.h"

class LoadingScene : public Scene {
public:
	SCENE_TYPE(LoadingScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	uint8_t _loading_step = 0;

	void _load_sprite(size_t u_start, size_t u_count, const std::string& file, CSpriteCollection& collection);
	void _load_scenes();
	void _load_interface();
	void _load_male_game_models();
	void _load_female_game_models();
	void _load_items();
	void _load_register_maps();
	void _load_sounds();
};
