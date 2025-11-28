#include "Scene.h"
#include "Application.h"
#include "Game.h"
#include "Sprite.h"
#include "SceneManager.h"

Scene::Scene()
	: m_game(Application::GetLayer<Game>()),
	m_sprites(m_game.m_sprites),
	m_modelSprites(m_game.m_modelSprites),
	m_sceneManager(*m_game.m_pSceneManager.get()),
	m_mapTiles(m_game.m_mapTiles),
	m_itemMetadata(m_game.m_itemMetadata),
	m_entities(m_game.m_entities)
{
}
