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
	void on_attach() override;
	void on_detach() override;
	void on_update() override;
	void on_render() override;
	void on_event(Event&) override;

	CSpriteCollection sprites{};
	CSpriteCollection model_sprites{};
	CSpriteCollection map_tiles{};
	std::vector<ItemMetadataEntry> item_metadata{};
	std::unique_ptr<SceneManager> scene_manager{ nullptr };

	// Replace with entity manager later
	std::vector<std::unique_ptr<Entity>> entities{};

	// Periodic Timer thread
	std::atomic<bool> is_running{ true };
	std::thread periodic_timer{};

	bool hardware_cursor = true;
};

