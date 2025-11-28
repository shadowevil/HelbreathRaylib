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

    virtual void OnInitialize() = 0;
    virtual void OnUninitialize() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;

protected:
    Game& m_game;
	SceneManager& m_sceneManager;
    CSpriteCollection& m_sprites;
	CSpriteCollection& m_modelSprites;
	CSpriteCollection& m_mapTiles;
	std::vector<ItemMetadataEntry>& m_itemMetadata;
	std::vector<std::unique_ptr<Entity>>& m_entities;
};