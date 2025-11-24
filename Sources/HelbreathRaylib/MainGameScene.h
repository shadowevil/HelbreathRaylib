#pragma once
#include "Scene.h"
#include "entities.h"
#include "CMap.h"
#include <unordered_set>
#include "Helbreath.h"

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

class MainGameScene : public core::Scene {
public:
	using Scene::Scene;
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

protected:
	Helbreath& m_application = Helbreath::Instance();

private:
	Player* m_player{};
	Camera2D m_camera{};
	std::unique_ptr<CMapData> m_mapData{};

	void DrawObjectsWithShadow();
	void DrawAll(const std::vector<DrawEntry>& list);
};
