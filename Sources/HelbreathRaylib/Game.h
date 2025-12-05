#pragma once
#include <vector>
#include "SceneManager.h"
#include "ApplicationLayer.h"
#include "Sprite.h"
#include "ItemMetadata.h"
#include "entity.h"
#include "Platform/IPlatformServices.h"
#include "Platform/ITimerService.h"
#include <atomic>

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

	// Platform services (accessed via Application)
	IPlatformServices* _platform{ nullptr };

	// Timer ID for periodic events
	TimerID _sprite_unload_timer{ 0 };

	bool hardware_cursor = true;
};

