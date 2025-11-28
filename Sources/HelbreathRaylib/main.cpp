#include <memory>
#include "Application.h"
#include "global_constants.h"
#include "Game.h"

int main() {
	SetTraceLogLevel(LOG_ERROR);
	WindowSpec spec;
	spec.Title = "Helbreath Raylib";
	spec.Width = constant::BASE_WIDTH;
	spec.Height = constant::BASE_HEIGHT;
	spec.Flags = WindowFlags::Resizable | WindowFlags::VSync | WindowFlags::MSAA_4X | WindowFlags::Upscaled;
	spec.TargetFPS = 60;
	if (!Application::CreateAppWindow(spec))
		return -1;

	Application::PushLayer<Game>();

	Application::Run();
}