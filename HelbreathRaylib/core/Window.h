#pragma once
#include "../Dependencies/Includes/raylib_include.h"
#include "Event.h"

namespace core {

	// Window properties structure
	struct WindowProperties {
		std::string title;
		int32_t width;
		int32_t height;
		bool vsync;
		bool resizable;
		bool fullscreen;

		WindowProperties(const std::string& title = "Helbreath",
			int32_t width = 800,
			int32_t height = 600,
			bool vsync = true,
			bool resizable = false,
			bool fullscreen = false)
			: title(title), width(width), height(height),
			vsync(vsync), resizable(resizable), fullscreen(fullscreen) {}
	};

	// Window class - handles window creation and events
	// Completely decoupled from the Application
	class Window {
	public:
		Window(const WindowProperties& props);
		~Window();

		// Delete copy constructor and assignment
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Update the window and poll events
		void PollEvents();

		// Check if window should close
		bool ShouldClose() const;

		// Getters
		int32_t GetWidth() const { return m_data.width; }
		int32_t GetHeight() const { return m_data.height; }
		const std::string& GetTitle() const { return m_data.title; }
		bool IsVSync() const { return m_data.vsync; }
		bool IsResizable() const { return m_data.resizable; }
		bool IsFullscreen() const { return m_data.fullscreen; }
		bool IsMinimized() const { return m_data.minimized; }
		bool IsMaximized() const { return m_data.maximized; }
		bool IsFocused() const { return m_data.focused; }

		// Setters
		void SetTitle(const std::string& title);
		void SetVSync(bool enabled);
		void SetSize(int32_t width, int32_t height);
		void SetFullscreen(bool enabled);

		// Event callback
		void SetEventCallback(const EventCallbackFn& callback) { m_data.eventCallback = callback; }

#ifdef _WIN32
		// Windows-specific handle getter
		void* GetNativeWindow() const;
#endif

	private:
		void Init(const WindowProperties& props);
		void Shutdown();
		void UpdateWindowState();

		struct WindowData {
			std::string title;
			int32_t width;
			int32_t height;
			bool vsync;
			bool resizable;
			bool fullscreen;
			bool minimized;
			bool maximized;
			bool focused;

			EventCallbackFn eventCallback;

			// Previous state tracking for event detection
			int32_t lastWidth;
			int32_t lastHeight;
			bool lastMinimized;
			bool lastMaximized;
			bool lastFocused;
			Vector2 lastMousePosition;
		};

		WindowData m_data;
	};

} // namespace core
