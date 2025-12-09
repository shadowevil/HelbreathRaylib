#include "Scenes.h"

void ExitGameScene::on_initialize()
{

}

void ExitGameScene::on_uninitialize()
{

}

void ExitGameScene::on_update()
{
	// Static timer to track elapsed time
	static double ExitTimer = raylib::GetTime();

	// Exit after 10 seconds
	if(rlx::HasElapsed(ExitTimer, 10.0))
	{
		Application::exit();
	}

	// Exit on mouse click or ESC after 3 seconds
	if (rlx::HasElapsed(ExitTimer, 3.0) && (raylib::IsMouseButtonPressed(raylib::MOUSE_BUTTON_LEFT) || raylib::IsKeyPressed(raylib::KEY_ESCAPE)))
	{
		Application::exit();
	}
}

void ExitGameScene::on_render()
{
	sprites[SPRID_EXITSCREEN]->draw(0, 0, SPR_EXITSCREEN::BACKGROUND);
	sprites[SPRID_EXITSCREEN]->draw(254, 120, SPR_EXITSCREEN::OVERLAY);
}
