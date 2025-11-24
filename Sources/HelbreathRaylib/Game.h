#pragma once
#include <vector>
#include "SceneManager.h"
#include "Layer.h"
#include "Helbreath.h"
#include "Sprite.h"
#include "ItemMetadata.h"
#include "entity.h"

class Game : public core::Layer {
public:
	Game(core::Application& app) : core::Layer(app) {}
	void OnPulse();
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnRender_AfterUpscale() override;

	CSpriteCollection m_sprites{};
	CSpriteCollection m_modelSprites{};
	CSpriteCollection m_mapTiles{};
	std::vector<ItemMetadataEntry> m_itemMetadata{};
	std::unique_ptr<core::SceneManager> m_pSceneManager{ nullptr };
	Helbreath& m_application = Helbreath::Instance();

	// Replace with entity manager later
	std::vector<std::unique_ptr<Entity>> m_entities{};

	bool HardwareCursor = true;
};

