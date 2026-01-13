#pragma once
#include "Scene.h"
#include "entities.h"
#include "GameSceneCommon.h"
#include <stdexcept>
#include "Application.h"

// Data passed from character selection to main game
struct GameEntryData {
	PlayerAppearance appearance{};
	PlayerEntityStats stats{};
	std::string character_name{};
};

class MainGameScene : public Scene {
public:
	SCENE_TYPE(MainGameScene)

	explicit MainGameScene(const GameEntryData& entry_data);

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	GameEntryData _entry_data{};
	Player* _player{};
	raylib::Camera2D _camera{};
	std::unique_ptr<CMapData> _map_data{};

	// Currently targeted entity (nullptr if none)
	Entity* _targeted_entity = nullptr;

	// Safe getter for the current map's entity manager
	EntityManager& get_entity_manager() {
		if (!_map_data) {
			throw std::runtime_error("MainGameScene: Attempted to access entity manager before map was loaded");
		}
		return _map_data->get_entity_manager();
	}

	const EntityManager& get_entity_manager() const {
		if (!_map_data) {
			throw std::runtime_error("MainGameScene: Attempted to access entity manager before map was loaded");
		}
		return _map_data->get_entity_manager();
	}

	void _draw_objects_with_shadow();
	void _draw_all(const std::vector<DrawEntry>& list);
	void _update_targeting();
	Entity* _find_entity_at_mouse() const;
};
