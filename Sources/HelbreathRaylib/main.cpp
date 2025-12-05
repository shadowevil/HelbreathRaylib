#include <memory>
#include "Application.h"
#include "global_constants.h"
#include "Game.h"
#include "Platform/PlatformFactory.h"
#include "Platform/IPlatformInfo.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);

	// Create platform using factory
	auto platform = PlatformFactory::create();

	// Get platform-specific window dimensions
	auto caps = platform->getPlatformInfo().getCapabilities();

	WindowSpec Spec;
	Spec.Title = "Helbreath Raylib";
	Spec.Width = caps.defaultWindowWidth;
	Spec.Height = caps.defaultWindowHeight;
	Spec.Flags = WindowFlags::Resizable /*| WindowFlags::VSync*/ | WindowFlags::MSAA_4X | WindowFlags::Upscaled;
	Spec.TargetFPS = 0;

	if (!Application::create_app_window(Spec, std::move(platform)))
		return -1;

	Application::push_layer<Game>();

	Application::run();
}