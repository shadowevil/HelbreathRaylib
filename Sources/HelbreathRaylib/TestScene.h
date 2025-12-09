#pragma once
#define TEST_SCENE_ENABLED
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>
#include <stdexcept>

class Entity;

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

class TestScene : public Scene {
public:
	SCENE_TYPE(TestScene)

	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	Player* _player{};
	std::unique_ptr<CMapData> _map_data{};
	raylib::Camera2D _camera{};
	std::string _debug_text{};

	// Safe getter for the current map's entity manager
	EntityManager& get_entity_manager() {
		if (!_map_data) {
			throw std::runtime_error("TestScene: Attempted to access entity manager before map was loaded");
		}
		return _map_data->get_entity_manager();
	}

    void _draw_objects_with_shadow();
    void _draw_all(const std::vector<DrawEntry>& list);
};