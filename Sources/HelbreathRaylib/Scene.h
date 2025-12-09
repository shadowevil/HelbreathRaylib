#pragma once
#include "raylib_include.h"
#include "Sprite.h"
#include <vector>

class Game;
struct ItemMetadataEntry;
class Entity;

class Application;
class SceneManager;
class EntityManager;
class StaticEntityManager;
class SoundPlayer;

// Macro to define scene type - converts class name to string automatically
#define SCENE_TYPE(ClassName) \
    static constexpr SceneTypeId scene_type_id = #ClassName; \
    SceneTypeId get_type_id() const override { return scene_type_id; } \
    using Scene::Scene;

class Scene {
public:
    using SceneTypeId = const char*;

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
    virtual SceneTypeId get_type_id() const = 0;

protected:
    Game& game;
	SceneManager& scene_manager;
    CSpriteCollection& sprites;
	CSpriteCollection& model_sprites;
	CSpriteCollection& map_tiles;
	std::vector<ItemMetadataEntry>& item_metadata;
	StaticEntityManager& static_entity_manager;
	SoundPlayer& sound_player;
};