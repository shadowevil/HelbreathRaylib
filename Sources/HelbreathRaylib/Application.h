#pragma once
#include "raylib_include.h"
#include "Window.h"
#include "Layer.h"
#include <vector>
#include <memory>

namespace core {
    class Application {
    public:
        Application(const Window::Config& config) : m_config(config) {}
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void Run() {
            m_window.Create(m_config);
            InitializeComponents();

            for (auto& layer : m_layers) {
                layer->OnInitialize();
            }

            while (m_window.IsOpen() && !WindowShouldClose()) {
                for (auto& layer : m_layers) {
                    layer->OnUpdate();
                }

                BeginDrawing();
				ClearBackground(BLACK);
                for (auto& layer : m_layers) {
                    layer->OnRender();
                }
				EndDrawing();
            }

            for (auto& layer : m_layers) {
                layer->OnUninitialize();
            }

            m_window.Destroy();
        }

        void Close() {
            if (m_window.IsOpen()) {
                m_window.Destroy();
            }
        }

        template<typename T, typename... Args>
        T* PushLayer(Args&&... args) {
            static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");

            if (m_layers.empty()) {
                auto layer = std::make_unique<T>(*this, std::forward<Args>(args)...);
                T* ptr = layer.get();
                m_layers.push_back(std::move(layer));
                m_primaryLayer = ptr;
                m_primaryLayer->OnInitialize();
                return ptr;
            }

            return nullptr;
        }

        template<typename T, typename... Args>
        T* PushOverlay(Args&&... args) {
            static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");

            if (m_layers.empty()) {
                return nullptr;
            }

            auto layer = std::make_unique<T>(*this, std::forward<Args>(args)...);
            T* ptr = layer.get();
            m_layers.push_back(std::move(layer));
			ptr->OnInitialize();
            return ptr;
        }

        void PopOverlay() {
            if (m_layers.size() > 1) {
                m_layers.back()->OnUninitialize();
                m_layers.pop_back();
            }
        }

    protected:
        virtual void InitializeComponents() = 0;

        Window m_window;
        Window::Config m_config;
        std::vector<std::unique_ptr<Layer>> m_layers;
        Layer* m_primaryLayer = nullptr;
    };
}