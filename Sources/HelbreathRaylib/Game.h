#pragma once
#include <vector>
#include "SceneManager.h"
#include "ApplicationLayer.h"
#include "Sprite.h"
#include "ItemMetadata.h"
#include "entity.h"
#include <thread>
#include <atomic>
#include <chrono>

class Game : public ApplicationLayer {
public:
	Game() : ApplicationLayer("Game") {}
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnEvent(Event&) override;

	CSpriteCollection m_sprites{};
	CSpriteCollection m_modelSprites{};
	CSpriteCollection m_mapTiles{};
	std::vector<ItemMetadataEntry> m_itemMetadata{};
	std::unique_ptr<SceneManager> m_pSceneManager{ nullptr };

	// Replace with entity manager later
	std::vector<std::unique_ptr<Entity>> m_entities{};

	// Periodic Timer thread
	std::atomic<bool> m_isRunning{ true };
	std::thread m_perodicTimer{};

	bool HardwareCursor = true;
};

