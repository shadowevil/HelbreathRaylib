#include "raylib_include.h"
#include "core/Application.h"
#include "core/Layer.h"

// Example layer to demonstrate the layer system
class ExampleLayer : public core::Layer {
public:
	ExampleLayer()
		: Layer("ExampleLayer") {}

	void Initialize() override {
		Layer::Initialize();
		// Load resources here
	}

	void Uninitialize() override {
		// Unload resources here
		Layer::Uninitialize();
	}

	void Update(float deltaTime) override {
		// Update game logic here
	}

	void Render() override {
		// Render game content here
		DrawText("Welcome to Helbreath Raylib!", 190, 200, 20, LIGHTGRAY);
		DrawText("Core Architecture Initialized", 190, 250, 20, GREEN);
		DrawText("- Event System", 190, 280, 18, WHITE);
		DrawText("- Layer System", 190, 300, 18, WHITE);
		DrawText("- Window Management", 190, 320, 18, WHITE);
		DrawText("- Application Singleton", 190, 340, 18, WHITE);
	}

	void OnEvent(core::Event& event) override {
		core::EventDispatcher dispatcher(event);

		// Handle specific events here
		dispatcher.Dispatch<core::KeyPressedEvent>([this](core::KeyPressedEvent& e) {
			if (e.GetKeyCode() == KEY_ESCAPE) {
				// Close application on ESC
				core::Application::Get().Close();
				return true;
			}
			return false;
		});

		dispatcher.Dispatch<core::WindowResizeEvent>([this](core::WindowResizeEvent& e) {
			// Handle window resize
			return false;
		});
	}
};

// Main Helbreath Application
class HelbreathApp : public core::Application {
public:
	HelbreathApp()
		: Application(core::WindowProperties("Helbreath Raylib", 800, 600, true, true, false))
	{
		PushLayer(std::make_unique<ExampleLayer>());
	}

	~HelbreathApp() override = default;

protected:
	void OnInitialize() override {
		// Custom initialization code here
	}

	void OnShutdown() override {
		// Custom shutdown code here
	}
};

// Entry point
namespace core {
	Application* CreateApplication() {
		return new HelbreathApp();
	}
}

int main() {
	auto app = core::CreateApplication();
	app->Run();
	delete app;
	return 0;
}