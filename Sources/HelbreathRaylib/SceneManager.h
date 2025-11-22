#pragma once
#include "raylib_include.h"
#include "Scene.h"
#include <memory>
#include <functional>

namespace core {
    class SceneManager {
    public:
        constexpr static float FAST_FADE_DURATION = 0.15f;
        constexpr static float DEFAULT_FADE_DURATION = 0.25f;
		constexpr static float SLOW_FADE_DURATION = 0.5f;

        enum class TransitionState {
            None,
            FadeOut,
            Switching,
            FadeIn
        };

        struct TransitionConfig {
            float fadeOutDuration = 0.5f;
            float fadeInDuration = 0.5f;
            Color fadeColor = BLACK;
        };

        explicit SceneManager() = default;

        template<typename T, typename... Args>
        void SetScene(Args&&... args) {
            static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");
            m_pendingScene = [... args = std::forward<Args>(args)]() mutable {
                return std::make_unique<T>(std::forward<Args>(args)...);
                };
            m_transitionState = TransitionState::FadeOut;
            m_transitionTime = 0.0f;
        }

        template<typename T, typename... Args>
        void PushOverlay(Args&&... args) {
            static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");

            if (!m_currentScene) return;

            auto overlay = std::make_unique<T>(std::forward<Args>(args)...);
            overlay->OnInitialize();
            m_overlay = std::move(overlay);
        }

        void PopOverlay() {
            if (m_overlay) {
                m_overlay->OnUninitialize();
                m_overlay.reset();
            }
        }

        void Update() {
            float dt = GetFrameTime();

            switch (m_transitionState) {
            case TransitionState::FadeOut:
                m_transitionTime += dt;
                if (m_transitionTime >= m_config.fadeOutDuration) {
                    m_transitionState = TransitionState::Switching;
                    m_transitionTime = 0.0f;
                }
                break;

            case TransitionState::Switching:
                if (m_currentScene) {
                    m_currentScene->OnUninitialize();
                }
                if (m_overlay) {
                    m_overlay->OnUninitialize();
                    m_overlay.reset();
                }
                m_currentScene = m_pendingScene();
                m_pendingScene = nullptr;
                m_currentScene->OnInitialize();
                m_transitionState = TransitionState::FadeIn;
                m_transitionTime = 0.0f;
                break;

            case TransitionState::FadeIn:
                m_transitionTime += dt;
                if (m_transitionTime >= m_config.fadeInDuration) {
                    m_transitionState = TransitionState::None;
                    m_transitionTime = 0.0f;
                }
                break;

            case TransitionState::None:
                if (m_currentScene) {
                    m_currentScene->OnUpdate();
                }
                if (m_overlay) {
                    m_overlay->OnUpdate();
                }
                break;
            }
        }

        void Render() {
            if (m_currentScene) {
                m_currentScene->OnRender();
            }
            if (m_overlay) {
                m_overlay->OnRender();
            }
            RenderTransition();
        }

        void SetTransitionConfig(const TransitionConfig& config) {
            m_config = config;
        }

        bool IsTransitioning() const {
            return m_transitionState != TransitionState::None;
        }

    private:
        void RenderTransition() {
            float alpha = 0.0f;

            switch (m_transitionState) {
            case TransitionState::FadeOut:
                alpha = m_transitionTime / m_config.fadeOutDuration;
                break;

            case TransitionState::Switching:
                alpha = 1.0f;
                break;

            case TransitionState::FadeIn:
                alpha = 1.0f - (m_transitionTime / m_config.fadeInDuration);
                break;

            case TransitionState::None:
                return;
            }

            Color fadeColor = m_config.fadeColor;
            fadeColor.a = static_cast<unsigned char>(alpha * 255);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), fadeColor);
        }

        std::unique_ptr<Scene> m_currentScene;
        std::unique_ptr<Scene> m_overlay;
        std::function<std::unique_ptr<Scene>()> m_pendingScene;
        TransitionState m_transitionState = TransitionState::None;
        TransitionConfig m_config;
        float m_transitionTime = 0.0f;
    };
}