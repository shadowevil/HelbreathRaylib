#pragma once
#include "raylib_include.h"
#include "Scene.h"
#include <memory>
#include <functional>

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
        float fade_out_duration = 0.5f;
        float fade_in_duration = 0.5f;
        Color fade_color = BLACK;
    };

    explicit SceneManager() = default;

    template<typename T, typename... Args>
    void set_scene(Args&&... args) {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");
        _pending_scene = [... args = std::forward<Args>(args)]() mutable {
            return std::make_unique<T>(std::forward<Args>(args)...);
            };
        _transition_state = TransitionState::FadeOut;
        _transition_time = 0.0f;
    }

    template<typename T, typename... Args>
    void push_overlay(Args&&... args) {
        static_assert(std::is_base_of_v<Scene, T>, "T must derive from Scene");

        if (!_current_scene) return;

        auto Overlay = std::make_unique<T>(std::forward<Args>(args)...);
        Overlay->on_initialize();
        _overlay = std::move(Overlay);
    }

    void pop_overlay() {
        if (_overlay) {
            _overlay->on_initialize();
            _overlay.reset();
        }
    }

    void update() {
        float Dt = GetFrameTime();

        switch (_transition_state) {
        case TransitionState::FadeOut:
            _transition_time += Dt;
            if (_transition_time >= _config.fade_out_duration) {
                _transition_state = TransitionState::Switching;
                _transition_time = 0.0f;
            }
            break;

        case TransitionState::Switching:
            if (_current_scene) {
                _current_scene->on_uninitialize();
            }
            if (_overlay) {
                _overlay->on_initialize();
                _overlay.reset();
            }
            _current_scene = _pending_scene();
            _pending_scene = nullptr;
            _current_scene->on_initialize();
            _transition_state = TransitionState::FadeIn;
            _transition_time = 0.0f;
            break;

        case TransitionState::FadeIn:
            _transition_time += Dt;
            if (_transition_time >= _config.fade_in_duration) {
                _transition_state = TransitionState::None;
                _transition_time = 0.0f;
            }
            break;

        case TransitionState::None:
            if (_current_scene) {
                _current_scene->on_update();
            }
            if (_overlay) {
                _overlay->on_update();
            }
            break;
        }
    }

    void render() {
        if (_current_scene) {
            _current_scene->on_render();
        }
        if (_overlay) {
            _overlay->on_render();
        }
        _render_transition();
    }

    void set_transition_config(const TransitionConfig& config) {
        _config = config;
    }

    bool is_transitioning() const {
        return _transition_state != TransitionState::None;
    }

private:
    void _render_transition() {
        float Alpha = 0.0f;

        switch (_transition_state) {
        case TransitionState::FadeOut:
            Alpha = _transition_time / _config.fade_out_duration;
            break;

        case TransitionState::Switching:
            Alpha = 1.0f;
            break;

        case TransitionState::FadeIn:
            Alpha = 1.0f - (_transition_time / _config.fade_in_duration);
            break;

        case TransitionState::None:
            return;
        }

        Color FadeColor = _config.fade_color;
        FadeColor.a = static_cast<unsigned char>(Alpha * 255);
        DrawRectangle(0, 0, constant::BASE_WIDTH, constant::BASE_HEIGHT, FadeColor);
    }

    std::unique_ptr<Scene> _current_scene;
    std::unique_ptr<Scene> _overlay;
    std::function<std::unique_ptr<Scene>()> _pending_scene;
    TransitionState _transition_state = TransitionState::None;
    TransitionConfig _config;
    float _transition_time = 0.0f;
};