#include "Game.h"
#include "raylib_include.h"
#include "Scenes.h"

void Game::OnInitialize()
{
	m_pSceneManager = std::make_unique<core::SceneManager>(m_application);

	m_pSceneManager->SetTransitionConfig({
		0.25f, // fadeOutDuration
		0.25f, // fadeInDuration
		BLACK // fadeColor
		});

	m_pSceneManager->SetScene<LoadingScene>();
}

void Game::OnUninitialize()
{
	m_pSceneManager.reset();
}

void Game::OnUpdate()
{
	m_pSceneManager->Update();
}

void Game::OnRender()
{
	m_pSceneManager->Render();
}
