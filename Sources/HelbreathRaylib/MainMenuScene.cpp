#include "Scenes.h"

void MainMenuScene::OnInitialize()
{

}

void MainMenuScene::OnUninitialize()
{

}

void MainMenuScene::OnUpdate()
{

}

void MainMenuScene::OnRender()
{
	m_sprites[SPRID_MAINMENUSCREEN]->Draw(0, 0, SPR_MAINMENUSCREEN::BACKGROUND);

}
