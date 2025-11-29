#pragma once
#include "raylib_include.h"
#include "Sprite.h"
#include <vector>

class Game;
struct ItemMetadataEntry;
class Entity;

class Application;
class SceneManager;

class Scene {
public:
    Scene();
    virtual ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;

    virtual void on_initialize() = 0;
    virtual void on_uninitialize() = 0;
    virtual void on_update() = 0;
    virtual void on_render() = 0;

protected:
    Game& game;
	SceneManager& scene_manager;
    CSpriteCollection& sprites;
	CSpriteCollection& model_sprites;
	CSpriteCollection& map_tiles;
	std::vector<ItemMetadataEntry>& item_metadata;
	std::vector<std::unique_ptr<Entity>>& entities;
};