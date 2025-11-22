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
	HideCursor();
	m_pSceneManager = std::make_unique<core::SceneManager>();

	m_pSceneManager->SetTransitionConfig({
		core::SceneManager::FAST_FADE_DURATION,
		core::SceneManager::FAST_FADE_DURATION,
		BLACK // fadeColor
		});

	core::FontManager::RegisterFont(FontFamily::Default,
		(constant::FONT_PATH / "default.ttf").string().c_str(),
		(constant::FONT_PATH /"default_bold.ttf").string().c_str()
	);

	core::FontManager::RegisterFont(FontFamily::Fancy,
		(constant::FONT_PATH / "black_chancery.ttf").string().c_str()
	);

	core::FontManager::RegisterFont(FontFamily::DialogTitle,
		(constant::FONT_PATH / "times_new_roman.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_bold.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_italic.ttf").string().c_str()
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
	// Draw mouse cursor
	if (!HardwareCursor) {
		auto [mouseX, mouseY] = core::GetMousePosition();

		if (m_sprites.Contains(SPRID_MOUSECURSOR)) {
			m_sprites[SPRID_MOUSECURSOR]->Draw(
				(int)mouseX,
				(int)mouseY,
				SPR_MOUSECURSOR::DEFAULT
			);
		}
	}
	core::DrawText(FontFamily::Default, 15, std::to_string(core::FramesPerSecond::GetFPS()).c_str(), 0, 0, WHITE, core::FontStyle::Regular);
}

void Game::OnRender_AfterUpscale()
{
	if (HardwareCursor) {
		// Draw mouse cursor
		if (m_sprites.Contains(SPRID_MOUSECURSOR)) {
			m_sprites[SPRID_MOUSECURSOR]->Draw(
				GetMouseX(),
				GetMouseY(),
				SPR_MOUSECURSOR::DEFAULT
			);
		}
	}
}
