#include "Scenes.h"
#include "json.hpp"

void CharacterSelectionScene::on_initialize()
{
	if(!rlx::Directory::Exists(constant::SAVE_GAME_PATH))
	{
		rlx::Directory::Create(constant::SAVE_GAME_PATH);
		return;
	}
	using json = nlohmann::json;

	auto SaveFiles = rlx::Directory::GetFilesWithExtension(constant::SAVE_GAME_PATH, ".jsave");
	for(auto& FilePath : SaveFiles)
	{
		auto JsaveData = json::parse(rlx::File::ReadAllText(FilePath));
		// Parse character data
	}
}

void CharacterSelectionScene::on_uninitialize()
{

}

void CharacterSelectionScene::on_update()
{
	if (_character_list.empty())
	{
		scene_manager.set_scene<CreateCharacterScene>();
		return;
	}
}

void CharacterSelectionScene::on_render()
{
	sprites[SPRID_CHARACTERSELECTSCREEN]->draw(0, 0, SPR_CHARACTERSELECTSCREEN::BACKGROUND);
}