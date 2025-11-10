#include "raylib_include.h"
#include "core/Application.h"
#include "Helbreath.h"

// Entry point
namespace core {
	Application* CreateApplication() {
		return new Helbreath();
	}
}

int main() {
	auto app = core::CreateApplication();
	app->Run();
	delete app;
	return 0;
}