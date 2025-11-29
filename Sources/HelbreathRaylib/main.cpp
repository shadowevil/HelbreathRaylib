#include <memory>
#include "Application.h"
#include "global_constants.h"
#include "Game.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);
	WindowSpec Spec;
	Spec.Title = "Helbreath Raylib";
	Spec.Width = constant::BASE_WIDTH;
	Spec.Height = constant::BASE_HEIGHT;
	Spec.Flags = WindowFlags::Resizable /*| WindowFlags::VSync*/ | WindowFlags::MSAA_4X | WindowFlags::Upscaled;
	Spec.TargetFPS = 0;
	if (!Application::create_app_window(Spec))
		return -1;

	Application::push_layer<Game>();

	Application::run();
}