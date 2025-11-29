#pragma once
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>
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
	using Scene::Scene;
	void on_initialize() override;
	void on_uninitialize() override;
	void on_update() override;
	void on_render() override;

private:
	Player* _player{};
	Camera2D _camera{};
	std::unique_ptr<CMapData> _map_data{};

	void _draw_objects_with_shadow();
	void _draw_all(const std::vector<DrawEntry>& list);
};
