#pragma once
#include "raylib_include.h"
#include <string>

namespace core {
    class Window {
    public:
        struct Config {
            std::string title = "raylib";
            int width = 800;
            int height = 600;
            unsigned int flags = 0;
            int targetFPS = 60;
        };

        Window() = default;
        explicit Window(const Config& config) { Create(config); }
        ~Window() { Destroy(); }

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        void Create(const Config& config) {
            if (m_isOpen) return;

            m_config = config;

            if (m_config.flags != 0)
                SetConfigFlags(m_config.flags);

            InitWindow(m_config.width, m_config.height, m_config.title.c_str());
            SetTargetFPS(m_config.targetFPS);

            m_isOpen = true;
        }

        void Destroy() {
            if (!m_isOpen) return;
            CloseWindow();
            m_isOpen = false;
        }

        void UpdateFlags(unsigned int newFlags) {
            if (newFlags == m_config.flags) return;

            if (RequiresRestart(m_config.flags, newFlags)) {
                Config temp = m_config;
                temp.flags = newFlags;
                Destroy();
                Create(temp);
            }
            else {
                m_config.flags = newFlags;
                ApplyRuntimeFlags(newFlags);
            }
        }

        void UpdateTitle(const std::string& title) {
            if (!m_isOpen) return;
            m_config.title = title;
            SetWindowTitle(title.c_str());
        }

        void UpdateSize(int width, int height) {
            if (!m_isOpen) return;
            m_config.width = width;
            m_config.height = height;
            SetWindowSize(width, height);
        }

        void UpdateTargetFPS(int fps) {
            if (!m_isOpen) return;
            m_config.targetFPS = fps;
            SetTargetFPS(fps);
        }

        bool IsOpen() const { return m_isOpen; }
        const Config& GetConfig() const { return m_config; }

    private:
        bool RequiresRestart(unsigned int oldFlags, unsigned int newFlags) const {
            constexpr unsigned int restartRequired =
                FLAG_VSYNC_HINT |
                FLAG_FULLSCREEN_MODE |
                FLAG_WINDOW_HIGHDPI |
                FLAG_WINDOW_TRANSPARENT |
                FLAG_MSAA_4X_HINT;

            unsigned int changed = oldFlags ^ newFlags;
            return (changed & restartRequired) != 0;
        }

        void ApplyRuntimeFlags(unsigned int flags) {
            if (flags & FLAG_WINDOW_RESIZABLE) SetWindowState(FLAG_WINDOW_RESIZABLE);
            else ClearWindowState(FLAG_WINDOW_RESIZABLE);

            if (flags & FLAG_WINDOW_UNDECORATED) SetWindowState(FLAG_WINDOW_UNDECORATED);
            else ClearWindowState(FLAG_WINDOW_UNDECORATED);

            if (flags & FLAG_WINDOW_TOPMOST) SetWindowState(FLAG_WINDOW_TOPMOST);
            else ClearWindowState(FLAG_WINDOW_TOPMOST);

            if (flags & FLAG_WINDOW_ALWAYS_RUN) SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
            else ClearWindowState(FLAG_WINDOW_ALWAYS_RUN);

            if (flags & FLAG_WINDOW_MINIMIZED) MinimizeWindow();
            if (flags & FLAG_WINDOW_MAXIMIZED) MaximizeWindow();
            else RestoreWindow();
        }

        Config m_config;
        bool m_isOpen = false;
    };
}