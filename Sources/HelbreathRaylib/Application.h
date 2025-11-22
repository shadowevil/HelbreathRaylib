#pragma once
#include "raylib_include.h"
#include "Window.h"
#include "Layer.h"
#include "SceneManager.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace core {
    struct FramesPerSecond {
    public:
        static void Update() {
            auto& inst = Instance();
            inst.frame_time += GetFrameTime();
            inst.frame_count++;
            if (inst.frame_time >= 1.0f) {
                inst.fps = inst.frame_count;
                inst.frame_count = 0;
                inst.frame_time -= 1.0f;
            }
		}

        static uint32_t GetFPS() {
            return Instance().fps;
        }

    private:
        static FramesPerSecond& Instance() {
            static FramesPerSecond fpsObj;
            return fpsObj;
		}

        FramesPerSecond() = default;
		~FramesPerSecond() = default;
        FramesPerSecond(const FramesPerSecond&) = delete;
        FramesPerSecond& operator=(const FramesPerSecond&) = delete;
        FramesPerSecond(FramesPerSecond&&) = delete;
		FramesPerSecond& operator=(FramesPerSecond&&) = delete;

        double frame_time = 0.0f;
        uint32_t fps = 0;
        uint32_t frame_count = 0;
    };

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
                if (IsWindowFocused()) {
                    FocusGained();
                }
                else {
                    FocusLost();
                }

                for (auto& layer : m_layers) {
                    layer->OnUpdate();
                }

                BeginDrawing();
                ClearBackground(BLACK);

                for (auto& layer : m_layers) {
                    layer->OnRender();
                }

                EndDrawing();

                FramesPerSecond::Update();
            }

            for (auto& layer : m_layers) {
                layer->OnUninitialize();
            }

            m_window.Destroy();
        }

        void RunUpscaled(int base_width, int base_height, int upscale_multi) {
            m_window.Create(m_config);
            InitializeComponents();

            RenderTexture2D target = LoadRenderTexture(base_width * upscale_multi, base_height * upscale_multi);
			SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
			m_isUpscaled = true;
            for (auto& layer : m_layers) {
                layer->OnInitialize();
            }

            while (m_window.IsOpen() && !m_exitRequested) {
                if(IsWindowFocused()) {
                    FocusGained();
                } else {
                    FocusLost();
				}

                for (auto& layer : m_layers) {
                    layer->OnUpdate();
                }

                rlx::BeginUpscaleRender(target, static_cast<float>(upscale_multi));
                ClearBackground(BLACK);

                for (auto& layer : m_layers) {
                    layer->OnRender();
                }

				rlx::EndUpscaleRender(target, BLACK,
                    [this]() { m_primaryLayer->OnRender_BeforeUpscale(); },
                    [this]() { m_primaryLayer->OnRender_AfterUpscale(); });

                FramesPerSecond::Update();
            }

            for (auto& layer : m_layers) {
                layer->OnUninitialize();
            }

			UnloadRenderTexture(target);

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

        template<typename T>
        T& GetOverlay() {
            static_assert(std::is_base_of_v<Layer, T>, "T must derive from layer");

            for (auto& l : m_layers)
            {
                if (auto ptr = dynamic_cast<T*>(l.get()))
                    return *ptr;
            }
            throw std::runtime_error("Layer does not exist!");
        }

        template<typename T>
        T& GetPrimaryLayer() {
            static_assert(std::is_base_of_v<Layer, T>, "T must derive from layer");
            if (!m_primaryLayer)
                throw std::runtime_error("No primary layer set!");
            auto ptr = dynamic_cast<T*>(m_primaryLayer);
            if (!ptr)
                throw std::runtime_error("Primary layer is not of the requested type!");
            return *ptr;
		}

        bool IsUpscaled() const {
            return m_isUpscaled;
		}

        void Exit() {
			m_exitRequested = true;
        }

    protected:
        virtual void InitializeComponents() = 0;
        virtual void FocusLost() {}
        virtual void FocusGained() {}

        Window m_window;
        Window::Config m_config;
        std::vector<std::unique_ptr<Layer>> m_layers;
        Layer* m_primaryLayer = nullptr;
        bool m_isUpscaled = false;
		bool m_exitRequested = false;
    };
}