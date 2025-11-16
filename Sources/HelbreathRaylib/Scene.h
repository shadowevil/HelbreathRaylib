#pragma once
#include "raylib_include.h"
#include "Sprite.h"

class Game;

namespace core {
    class Application;
	class SceneManager;

    class Scene {
    public:
        Scene(Application& app);
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
        Application& m_application;
        Game& m_game;
		SceneManager& m_sceneManager;
        CSpriteCollection& m_sprites;
    };
}