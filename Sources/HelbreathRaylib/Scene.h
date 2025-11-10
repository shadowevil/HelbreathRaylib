#pragma once
#include "raylib_include.h"

namespace core {
    class Application;

    class Scene {
    public:
        explicit Scene(Application& app) : m_application(app) {}
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
    };
}