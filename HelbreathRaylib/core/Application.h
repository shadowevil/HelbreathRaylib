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

		// Layer management - template methods for easy layer creation
		template<typename T, typename... Args>
		T* PushLayer(Args&&... args);

		template<typename T, typename... Args>
		T* PushOverlay(Args&&... args);

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
		bool m_initialized;
	};

	// Template implementations
	template<typename T, typename... Args>
	T* Application::PushLayer(Args&&... args) {
		static_assert(std::is_base_of<Layer, T>::value, "T must derive from Layer");

		auto layer = std::make_unique<T>(std::forward<Args>(args)...);
		T* layerPtr = layer.get();

		m_layerStack.PushLayer(std::move(layer));

		// Initialize the layer if application is already running
		if (m_initialized && !layerPtr->IsInitialized()) {
			layerPtr->Initialize();
		}

		return layerPtr;
	}

	template<typename T, typename... Args>
	T* Application::PushOverlay(Args&&... args) {
		static_assert(std::is_base_of<Layer, T>::value, "T must derive from Layer");

		auto overlay = std::make_unique<T>(std::forward<Args>(args)...);
		T* overlayPtr = overlay.get();

		m_layerStack.PushOverlay(std::move(overlay));

		// Initialize the overlay if application is already running
		if (m_initialized && !overlayPtr->IsInitialized()) {
			overlayPtr->Initialize();
		}

		return overlayPtr;
	}

} // namespace core
