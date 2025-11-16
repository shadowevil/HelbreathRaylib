#include "Scene.h"
#include "Application.h"
#include "Game.h"
#include "Sprite.h"
#include "SceneManager.h"

using namespace core;

core::Scene::Scene(Application& app)
	: m_application(app), m_game(app.GetPrimaryLayer<Game>()),
	m_sprites(m_game.m_sprites), m_sceneManager(*m_game.m_pSceneManager.get())
{
}
