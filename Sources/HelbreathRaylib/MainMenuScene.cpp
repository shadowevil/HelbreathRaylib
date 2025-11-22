#include "Scenes.h"
#include "Colors.h"

void MainMenuScene::OnInitialize()
{
	m_mainMenuButtonRects.resize(MainMenuButton::COUNT);	// height = 22, spacing = 15, start y = 178
	m_mainMenuButtonRects[MainMenuButton::LOGIN]		= { 385, 178 + ((22 + 15) * 0), 164, 22 };
	m_mainMenuButtonRects[MainMenuButton::NEW_ACCOUNT]	= { 385, 178 + ((22 + 15) * 1) + 1, 164, 22 };
	m_mainMenuButtonRects[MainMenuButton::EXIT]			= { 385, 178 + ((22 + 15) * 2) + 3, 164, 22 };
}

void MainMenuScene::OnUninitialize()
{

}

void MainMenuScene::OnUpdate()
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::LOGIN])) {

		}
		else if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::NEW_ACCOUNT])) {

		}
		else if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::EXIT])) {
			m_sceneManager.SetScene<ExitGameScene>();
		}
	}
}

void MainMenuScene::OnRender()
{
	m_sprites[SPRID_MAINMENUSCREEN]->Draw(0, 0, SPR_MAINMENUSCREEN::BACKGROUND);

	if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::LOGIN])) {
		m_sprites[SPRID_MAINMENUSCREEN]->Draw(m_mainMenuButtonRects[MainMenuButton::LOGIN].x, m_mainMenuButtonRects[MainMenuButton::LOGIN].y, SPR_MAINMENUSCREEN::BUTTON_1);
	}
	else if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::NEW_ACCOUNT])) {
		m_sprites[SPRID_MAINMENUSCREEN]->Draw(m_mainMenuButtonRects[MainMenuButton::NEW_ACCOUNT].x, m_mainMenuButtonRects[MainMenuButton::NEW_ACCOUNT].y, SPR_MAINMENUSCREEN::BUTTON_2);
	}
	else if (core::RectangleContainsMouse(m_mainMenuButtonRects[MainMenuButton::EXIT])) {
		m_sprites[SPRID_MAINMENUSCREEN]->Draw(m_mainMenuButtonRects[MainMenuButton::EXIT].x, m_mainMenuButtonRects[MainMenuButton::EXIT].y, SPR_MAINMENUSCREEN::BUTTON_3);
	}
}
