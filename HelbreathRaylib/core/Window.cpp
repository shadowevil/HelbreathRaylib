#include "Window.h"

namespace core {

	Window::Window(const WindowProperties& props) {
		Init(props);
	}

	Window::~Window() {
		Shutdown();
	}

	void Window::Init(const WindowProperties& props) {
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;
		m_data.vsync = props.vsync;
		m_data.resizable = props.resizable;
		m_data.fullscreen = props.fullscreen;
		m_data.minimized = false;
		m_data.maximized = false;
		m_data.focused = true;

		// Initialize previous state
		m_data.lastWidth = props.width;
		m_data.lastHeight = props.height;
		m_data.lastMinimized = false;
		m_data.lastMaximized = false;
		m_data.lastFocused = true;
		m_data.lastMousePosition = { 0.0f, 0.0f };

		// Set config flags before InitWindow
		uint32_t flags = 0;
		if (m_data.resizable)
			flags |= FLAG_WINDOW_RESIZABLE;
		if (m_data.vsync)
			flags |= FLAG_VSYNC_HINT;

		if (flags != 0)
			SetConfigFlags(flags);

		// Initialize the window
		InitWindow(m_data.width, m_data.height, m_data.title.c_str());

		// Apply fullscreen if needed
		if (m_data.fullscreen) {
			ToggleFullscreen();
		}

		// Set target FPS if vsync is not enabled
		if (!m_data.vsync) {
			SetTargetFPS(60);
		}
	}

	void Window::Shutdown() {
		CloseWindow();
	}

	void Window::PollEvents() {
		// Update window state
		UpdateWindowState();

		// Check for window resize
		int32_t currentWidth = GetScreenWidth();
		int32_t currentHeight = GetScreenHeight();
		if (currentWidth != m_data.lastWidth || currentHeight != m_data.lastHeight) {
			m_data.width = currentWidth;
			m_data.height = currentHeight;
			m_data.lastWidth = currentWidth;
			m_data.lastHeight = currentHeight;

			if (m_data.eventCallback) {
				WindowResizeEvent event(currentWidth, currentHeight);
				m_data.eventCallback(event);
			}
		}

		// Check for window minimize
		bool currentMinimized = IsWindowMinimized();
		if (currentMinimized != m_data.lastMinimized) {
			m_data.minimized = currentMinimized;
			m_data.lastMinimized = currentMinimized;

			if (m_data.eventCallback && currentMinimized) {
				WindowMinimizeEvent event;
				m_data.eventCallback(event);
			}
		}

		// Check for window maximize
		bool currentMaximized = IsWindowMaximized();
		if (currentMaximized != m_data.lastMaximized) {
			m_data.maximized = currentMaximized;
			m_data.lastMaximized = currentMaximized;

			if (m_data.eventCallback && currentMaximized) {
				WindowMaximizeEvent event;
				m_data.eventCallback(event);
			}
		}

		// Check for window focus
		bool currentFocused = IsWindowFocused();
		if (currentFocused != m_data.lastFocused) {
			m_data.focused = currentFocused;
			m_data.lastFocused = currentFocused;

			if (m_data.eventCallback) {
				if (currentFocused) {
					WindowFocusEvent event;
					m_data.eventCallback(event);
				}
				else {
					WindowLostFocusEvent event;
					m_data.eventCallback(event);
				}
			}
		}

		// Check for mouse movement
		Vector2 mousePos = GetMousePosition();
		if (mousePos.x != m_data.lastMousePosition.x || mousePos.y != m_data.lastMousePosition.y) {
			m_data.lastMousePosition = mousePos;

			if (m_data.eventCallback) {
				MouseMovedEvent event(mousePos.x, mousePos.y);
				m_data.eventCallback(event);
			}
		}

		// Check for mouse wheel
		float wheelMove = GetMouseWheelMove();
		if (wheelMove != 0.0f) {
			if (m_data.eventCallback) {
				MouseScrolledEvent event(0.0f, wheelMove);
				m_data.eventCallback(event);
			}
		}

		// Check for mouse buttons
		for (int32_t button = 0; button <= MOUSE_BUTTON_MIDDLE; button++) {
			if (IsMouseButtonPressed(button)) {
				if (m_data.eventCallback) {
					MouseButtonPressedEvent event(button);
					m_data.eventCallback(event);
				}
			}
			if (IsMouseButtonReleased(button)) {
				if (m_data.eventCallback) {
					MouseButtonReleasedEvent event(button);
					m_data.eventCallback(event);
				}
			}
		}

		// Check for keyboard keys
		// Note: raylib doesn't have a simple way to check all keys, so we check common ones
		// For a more complete implementation, you might want to check specific keys you care about
		int32_t key = GetKeyPressed();
		while (key > 0) {
			if (m_data.eventCallback) {
				KeyPressedEvent event(key, false);
				m_data.eventCallback(event);
			}
			key = GetKeyPressed();
		}

		// Check for key releases - we track this with a common set of keys
		static const int32_t commonKeys[] = {
			KEY_SPACE, KEY_ESCAPE, KEY_ENTER, KEY_TAB, KEY_BACKSPACE,
			KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
			KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
			KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
			KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
			KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
		};

		for (int32_t k : commonKeys) {
			if (IsKeyReleased(k)) {
				if (m_data.eventCallback) {
					KeyReleasedEvent event(k);
					m_data.eventCallback(event);
				}
			}
		}
	}

	bool Window::ShouldClose() const {
		if (WindowShouldClose() && m_data.eventCallback) {
			WindowCloseEvent event;
			m_data.eventCallback(event);
		}
		return WindowShouldClose();
	}

	void Window::SetTitle(const std::string& title) {
		m_data.title = title;
		SetWindowTitle(title.c_str());
	}

	void Window::SetVSync(bool enabled) {
		m_data.vsync = enabled;
		// Note: raylib doesn't have a runtime vsync toggle
		// You would need to recreate the window or set it before InitWindow
	}

	void Window::SetSize(int32_t width, int32_t height) {
		m_data.width = width;
		m_data.height = height;
		SetWindowSize(width, height);
	}

	void Window::SetFullscreen(bool enabled) {
		if (m_data.fullscreen != enabled) {
			m_data.fullscreen = enabled;
			ToggleFullscreen();
		}
	}

	void Window::UpdateWindowState() {
		// This is called every frame to sync internal state with raylib
		m_data.minimized = IsWindowMinimized();
		m_data.maximized = IsWindowMaximized();
		m_data.focused = IsWindowFocused();
	}

#ifdef _WIN32
	void* Window::GetNativeWindow() const {
		// Get the native window handle on Windows
		// This requires accessing raylib internals or platform-specific code
		// For now, we return nullptr as a placeholder
		return nullptr;
	}
#endif

} // namespace core
