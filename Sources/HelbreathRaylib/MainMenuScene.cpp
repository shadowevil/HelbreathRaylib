#include "Scenes.h"
#include "Colors.h"

void MainMenuScene::on_initialize()
{
	_main_menu_button_rects.resize(MainMenuButton::COUNT);	// height = 22, spacing = 15, start y = 178
	_main_menu_button_rects[MainMenuButton::LOGIN]		= rlx::Rectangle<short>( 385, 178 + ((22 + 15) * 0), 164, 22 );
	_main_menu_button_rects[MainMenuButton::NEW_ACCOUNT]	= rlx::Rectangle<short>( 385, 178 + ((22 + 15) * 1) + 1, 164, 22 );
	_main_menu_button_rects[MainMenuButton::EXIT]			= rlx::Rectangle<short>( 385, 178 + ((22 + 15) * 2) + 3, 164, 22 );
}

void MainMenuScene::on_uninitialize()
{

}

void MainMenuScene::on_update()
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::LOGIN])) {
			scene_manager.set_scene<CharacterSelectionScene>();
		}
		else if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::NEW_ACCOUNT])) {

		}
		else if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::EXIT])) {
			scene_manager.set_scene<ExitGameScene>();
		}
	}
}

void MainMenuScene::on_render()
{
	sprites[SPRID_MAINMENUSCREEN]->draw(0, 0, SPR_MAINMENUSCREEN::BACKGROUND);

	if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::LOGIN])) {
		sprites[SPRID_MAINMENUSCREEN]->draw(_main_menu_button_rects[MainMenuButton::LOGIN].x, _main_menu_button_rects[MainMenuButton::LOGIN].y, SPR_MAINMENUSCREEN::BUTTON_1);
	}
	else if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::NEW_ACCOUNT])) {
		sprites[SPRID_MAINMENUSCREEN]->draw(_main_menu_button_rects[MainMenuButton::NEW_ACCOUNT].x, _main_menu_button_rects[MainMenuButton::NEW_ACCOUNT].y, SPR_MAINMENUSCREEN::BUTTON_2);
	}
	else if (rlx::rectangle_contains_mouse(_main_menu_button_rects[MainMenuButton::EXIT])) {
		sprites[SPRID_MAINMENUSCREEN]->draw(_main_menu_button_rects[MainMenuButton::EXIT].x, _main_menu_button_rects[MainMenuButton::EXIT].y, SPR_MAINMENUSCREEN::BUTTON_3);
	}
}
