#pragma once
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>
#include <stdexcept>
#include "Application.h"

struct DrawEntry
{
	int sortY;
	enum Type { ObjectShadow, Object, EntityShadow, Entity } type;
	const CTile* tile;
	const ::Entity* entity;
	int pX, pY;
};

inline std::unordered_set<uint32_t> ShadowlessObjects = {
	242, 243, 244
};

class MainGameScene : public Scene {
public:
	SCENE_TYPE(MainGameScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	Player* _player{};
	raylib::Camera2D _camera{};
	std::unique_ptr<CMapData> _map_data{};

	// Safe getter for the current map's entity manager
	EntityManager& get_entity_manager() {
		if (!_map_data) {
			throw std::runtime_error("MainGameScene: Attempted to access entity manager before map was loaded");
		}
		return _map_data->get_entity_manager();
	}

	void _draw_objects_with_shadow();
	void _draw_all(const std::vector<DrawEntry>& list);
};
