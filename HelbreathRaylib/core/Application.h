#pragma once
#include "raylib_include.h"
#include "Window.h"
#include "Layer.h"
#include "Event.h"

namespace core {

	// Application class - Singleton pattern
	// This is the main application class that owns the window and manages layers
	class Application {
	public:
		Application(const WindowProperties& props = WindowProperties());
		virtual ~Application();

		// Delete copy constructor and assignment
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		// Main application loop
		void Run();

		// Stop the application
		void Close();

		// Layer management
		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		// Event handling
		void OnEvent(Event& event);

		// Getters
		Window& GetWindow() { return *m_window; }
		LayerStack& GetLayerStack() { return m_layerStack; }
		float GetDeltaTime() const { return m_deltaTime; }
		float GetTime() const { return m_time; }

		// Singleton access
		static Application& Get() { return *s_instance; }

	protected:
		// Override this in derived classes for custom initialization
		virtual void OnInitialize() {}
		virtual void OnShutdown() {}

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_instance;

		std::unique_ptr<Window> m_window;
		LayerStack m_layerStack;
		bool m_running;
		bool m_minimized;
		float m_time;
		float m_deltaTime;
		float m_lastFrameTime;
	};

	// To be defined in client application
	Application* CreateApplication();

} // namespace core
