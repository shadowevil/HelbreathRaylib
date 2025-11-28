#pragma once
#define TEST_SCENE_ENABLED
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>

class ::Entity;

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
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

private:
	Player* m_player{};
	std::unique_ptr<CMapData> m_mapData{};
	Camera2D m_camera{};
	std::string m_debugText{};

    void DrawObjectsWithShadow();
    void DrawAll(const std::vector<DrawEntry>& list);
};