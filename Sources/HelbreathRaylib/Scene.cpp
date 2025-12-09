#include "Scene.h"
#include "Application.h"
#include "Game.h"
#include "Sprite.h"
#include "SceneManager.h"

Scene::Scene()
	: game(Application::get_layer<Game>()),
	sprites(game.sprites),
	model_sprites(game.model_sprites),
	scene_manager(*game.scene_manager.get()),
	map_tiles(game.map_tiles),
	item_metadata(game.item_metadata),
	static_entity_manager(*game.static_entity_manager.get()),
	sound_player(*game.sound_player.get())
{
}
