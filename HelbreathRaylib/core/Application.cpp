#include "Application.h"

namespace core {

	Application* Application::s_instance = nullptr;

	Application::Application(const WindowProperties& props)
		: m_running(true), m_minimized(false), m_time(0.0f), m_deltaTime(0.0f), m_lastFrameTime(0.0f) {

		// Ensure only one instance exists
		if (s_instance != nullptr) {
			throw std::runtime_error("Application already exists!");
		}
		s_instance = this;

		// Create the window
		m_window = std::make_unique<Window>(props);
		m_window->SetEventCallback([this](Event& e) { OnEvent(e); });

		// Call user initialization
		OnInitialize();

		// Initialize all layers
		m_layerStack.InitializeAll();
	}

	Application::~Application() {
		// Call user shutdown
		OnShutdown();

		// Uninitialize all layers
		m_layerStack.UninitializeAll();

		s_instance = nullptr;
	}

	void Application::Run() {
		m_lastFrameTime = static_cast<float>(GetTime());

		while (m_running && !m_window->ShouldClose()) {
			// Calculate delta time
			float currentTime = static_cast<float>(GetTime());
			m_deltaTime = currentTime - m_lastFrameTime;
			m_lastFrameTime = currentTime;
			m_time = currentTime;

			// Poll window events
			m_window->PollEvents();

			// Update and render layers if not minimized
			if (!m_minimized) {
				// Update all layers
				m_layerStack.UpdateAll(m_deltaTime);

				// Render all layers
				BeginDrawing();
				ClearBackground(BLACK);
				m_layerStack.RenderAll();
				EndDrawing();
			}
		}
	}

	void Application::Close() {
		m_running = false;
	}

	void Application::PushLayer(std::unique_ptr<Layer> layer) {
		m_layerStack.PushLayer(std::move(layer));
		// Initialize the layer if the application is already running
		if (m_running) {
			m_layerStack.InitializeAll();
		}
	}

	void Application::PushOverlay(std::unique_ptr<Layer> overlay) {
		m_layerStack.PushOverlay(std::move(overlay));
		// Initialize the overlay if the application is already running
		if (m_running) {
			m_layerStack.InitializeAll();
		}
	}

	void Application::PopLayer(Layer* layer) {
		m_layerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer* overlay) {
		m_layerStack.PopOverlay(overlay);
	}

	void Application::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);

		// Handle application-level events
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { return OnWindowResize(e); });

		// Propagate events to layers (from top to bottom)
		m_layerStack.PropagateEvent(event);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		return false;
	}

} // namespace core
