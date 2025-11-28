#include "Scenes.h"
#include "json.hpp"

void CharacterSelectionScene::OnInitialize()
{
	if(!rlx::Directory::Exists(constant::SAVE_GAME_PATH))
	{
		rlx::Directory::Create(constant::SAVE_GAME_PATH);
		return;
	}
	using json = nlohmann::json;

	auto saveFiles = rlx::Directory::GetFilesWithExtension(constant::SAVE_GAME_PATH, ".jsave");
	for(auto& filePath : saveFiles)
	{
		auto jsaveData = json::parse(rlx::File::ReadAllText(filePath));
		// Parse character data
	}
}

void CharacterSelectionScene::OnUninitialize()
{

}

void CharacterSelectionScene::OnUpdate()
{
	if (m_characterList.empty())
	{
		m_sceneManager.SetScene<CreateCharacterScene>();
		return;
	}
}

void CharacterSelectionScene::OnRender()
{
	m_sprites[SPRID_CHARACTERSELECTSCREEN]->Draw(0, 0, SPR_CHARACTERSELECTSCREEN::BACKGROUND);
}