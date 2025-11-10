#pragma once
#include "raylib_include.h"
#include "Event.h"

namespace core {

	// Window flags for configuration
	enum WindowFlags : uint32_t {
		WINDOW_NONE = 0,
		WINDOW_VSYNC = 1 << 0,
		WINDOW_RESIZABLE = 1 << 1,
		WINDOW_FULLSCREEN = 1 << 2,
		WINDOW_BORDERLESS = 1 << 3,
		WINDOW_HIDDEN = 1 << 4,
		WINDOW_MINIMIZED = 1 << 5,
		WINDOW_MAXIMIZED = 1 << 6,
		WINDOW_UNFOCUSED = 1 << 7,
		WINDOW_TOPMOST = 1 << 8,
		WINDOW_HIGHDPI = 1 << 9,
		WINDOW_ALWAYS_RUN = 1 << 10,
		WINDOW_TRANSPARENT = 1 << 11
	};

	// Bitwise OR operator for WindowFlags
	inline WindowFlags operator|(WindowFlags a, WindowFlags b) {
		return static_cast<WindowFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	// Bitwise AND operator for WindowFlags
	inline WindowFlags operator&(WindowFlags a, WindowFlags b) {
		return static_cast<WindowFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	// Window properties structure
	struct WindowProperties {
		std::string title;
		int32_t width;
		int32_t height;
		uint32_t flags;

		WindowProperties(const std::string& title = "Helbreath",
			int32_t width = 800,
			int32_t height = 600,
			uint32_t flags = WINDOW_VSYNC)
			: title(title), width(width), height(height), flags(flags) {}

		// Helper methods to check flags
		bool HasFlag(WindowFlags flag) const { return (flags & flag) != 0; }
		bool IsVSync() const { return HasFlag(WINDOW_VSYNC); }
		bool IsResizable() const { return HasFlag(WINDOW_RESIZABLE); }
		bool IsFullscreen() const { return HasFlag(WINDOW_FULLSCREEN); }
		bool IsBorderless() const { return HasFlag(WINDOW_BORDERLESS); }
		bool IsTransparent() const { return HasFlag(WINDOW_TRANSPARENT); }
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
