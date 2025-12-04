#pragma once
#define TEST_SCENE_ENABLED
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>

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
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	Player* _player{};
	std::unique_ptr<CMapData> _map_data{};
	Camera2D _camera{};
	std::string _debug_text{};

    void _draw_objects_with_shadow();
    void _draw_all(const std::vector<DrawEntry>& list);
};