#include "Scenes.h"
#include <algorithm>

void LoadingScene::OnInitialize()
{
	CSpriteLoader::OpenPAK("Scenes.pak", [&](CSpriteLoader& loader) {
		m_sprites[SPRID_LOADINGSCREEN] = loader.GetSprite(SPR_LOADINGSCREEN::PAK_INDEX);
		});
}

void LoadingScene::OnUninitialize()
{

}

void LoadingScene::OnUpdate()
{
	if (m_loadingStep >= 100)
	{
		m_sceneManager.SetScene<MainMenuScene>();
		return;
	}

	switch (m_loadingStep) {
	case 0:
		CSpriteLoader::OpenPAK("Scenes.pak", [&](CSpriteLoader& loader) {
			m_sprites[SPRID_MAINMENUSCREEN] = loader.GetSprite(SPR_MAINMENUSCREEN::PAK_INDEX);
			});
		break;
	}

	m_loadingStep = (uint8_t)std::min(m_loadingStep + 1, 101);
}

void LoadingScene::OnRender()
{
	m_sprites[SPRID_LOADINGSCREEN]->Draw(0, 0, SPR_LOADINGSCREEN::BACKGROUND);
}
