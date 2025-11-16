#include "Game.h"
#include "raylib_include.h"
#include "Scenes.h"
#include "FontManager.h"
#include "global_constants.h"

void Game::OnPulse()
{
	for(auto& [index, sprite] : m_sprites) {
		sprite->UnloadIfUnused(GetTime(), constant::SPRITE_UNLOAD_TIMEOUT_SECONDS);
	}
}

void Game::OnInitialize()
{
	//HideCursor();
	m_pSceneManager = std::make_unique<core::SceneManager>(m_application);

	m_pSceneManager->SetTransitionConfig({
		core::SceneManager::FAST_FADE_DURATION,
		core::SceneManager::FAST_FADE_DURATION,
		BLACK // fadeColor
		});

	core::FontManager::RegisterFont(FontFamily::Default,
		path_combine(get_executable_dir(), constant::FONT_PATH, "default.ttf").c_str(),
		path_combine(get_executable_dir(), constant::FONT_PATH, "default_bold.ttf").c_str()
	);

	core::FontManager::RegisterFont(FontFamily::Fancy,
		path_combine(get_executable_dir(), constant::FONT_PATH, "black_chancery.ttf").c_str()
	);

	core::FontManager::RegisterFont(FontFamily::DialogTitle,
		path_combine(get_executable_dir(), constant::FONT_PATH, "times_new_roman.ttf").c_str(),
		path_combine(get_executable_dir(), constant::FONT_PATH, "times_new_roman_bold.ttf").c_str(),
		path_combine(get_executable_dir(), constant::FONT_PATH, "times_new_roman_italic.ttf").c_str()
	);

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
	core::DrawText(FontFamily::Default, 14, std::to_string(core::FramesPerSecond::GetFPS()).c_str(), 0, 0, WHITE, core::FontStyle::Regular);
}
