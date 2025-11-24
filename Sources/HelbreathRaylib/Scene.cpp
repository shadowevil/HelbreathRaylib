#include "Scene.h"
#include "Application.h"
#include "Game.h"
#include "Sprite.h"
#include "SceneManager.h"

using namespace core;

core::Scene::Scene()
	: m_application(Helbreath::Instance()),
	m_game(m_application.GetPrimaryLayer<Game>()),
	m_sprites(m_game.m_sprites),
	m_modelSprites(m_game.m_modelSprites),
	m_sceneManager(*m_game.m_pSceneManager.get()),
	m_mapTiles(m_game.m_mapTiles),
	m_itemMetadata(m_game.m_itemMetadata),
	m_entities(m_game.m_entities)
{
}
