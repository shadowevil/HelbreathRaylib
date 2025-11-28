#include "Scenes.h"

void ExitGameScene::OnInitialize()
{

}

void ExitGameScene::OnUninitialize()
{

}

void ExitGameScene::OnUpdate()
{
	// Static timer to track elapsed time
	static double exitTimer = GetTime();

	// Exit after 10 seconds
	if(rlx::HasElapsed(exitTimer, 10.0))
	{
		Application::Exit();
	}

	// Exit on mouse click or ESC after 3 seconds
	if (rlx::HasElapsed(exitTimer, 3.0) && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ESCAPE)))
	{
		Application::Exit();
	}
}

void ExitGameScene::OnRender()
{
	m_sprites[SPRID_EXITSCREEN]->Draw(0, 0, SPR_EXITSCREEN::BACKGROUND);
	m_sprites[SPRID_EXITSCREEN]->Draw(254, 120, SPR_EXITSCREEN::OVERLAY);
}
