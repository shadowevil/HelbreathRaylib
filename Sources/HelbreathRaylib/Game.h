#pragma once
#include <vector>
#include "SceneManager.h"
#include "Layer.h"
#include "Helbreath.h"
#include "Sprite.h"
#include "ItemMetadata.h"

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

	bool HardwareCursor = true;
};

