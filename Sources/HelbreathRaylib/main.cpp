#include <memory>
#include "Application.h"
#include "global_constants.h"
#include "Game.h"
#include "Platform/PlatformFactory.h"
#include "Platform/IPlatformInfo.h"

int main() {
	raylib::SetTraceLogLevel(raylib::LOG_ERROR);

	// Create platform using factory
	auto platform = PlatformFactory::create();

	// Get platform-specific window dimensions
	auto caps = platform->getPlatformInfo().getCapabilities();

	WindowSpec Spec;
	Spec.Title = "Helbreath Raylib";
	Spec.Width = caps.defaultWindowWidth;
	Spec.Height = caps.defaultWindowHeight;

	// On web, disable resizable flag to prevent Emscripten from auto-resizing the canvas
	// We'll handle scaling via CSS in the shell.html instead
	WindowFlags flags = WindowFlags::MSAA_4X | WindowFlags::Upscaled;

	//if (platform->getPlatformType() != PlatformType::Web)
	//	flags = flags | WindowFlags::Resizable;

	Spec.Flags = flags | WindowFlags::VSync;
	Spec.TargetFPS = 0;

	if (!Application::create_app_window(Spec, std::move(platform)))
		return -1;

	Application::push_layer<Game>();

	Application::run();
}

#ifdef _WIN32
int APIENTRY WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	return main();
}
#endif
