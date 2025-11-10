#pragma once
#include "raylib_include.h"

namespace core {
    class Application;

    class Layer {
    public:
        Layer(Application& app) : m_application(app) {}
        virtual ~Layer() = default;

        Layer(const Layer&) = delete;
        Layer& operator=(const Layer&) = delete;
        Layer(Layer&&) = delete;
        Layer& operator=(Layer&&) = delete;

        virtual void OnInitialize() = 0;
        virtual void OnUninitialize() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender() = 0;

    protected:
        Application& m_application;
    };
}