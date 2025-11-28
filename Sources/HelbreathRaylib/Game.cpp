#include "Game.h"
#include "raylib_include.h"
#include "Scenes.h"
#include "FontSystem.h"
#include "global_constants.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "GameEvents.h"

void Game::OnAttach()
{
	HideCursor();
	rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);

	m_pSceneManager = std::make_unique<SceneManager>();

	m_pSceneManager->SetTransitionConfig({
		SceneManager::FAST_FADE_DURATION,
		SceneManager::FAST_FADE_DURATION,
		BLACK // fadeColor
		});

	FontSystem::RegisterFont(FontFamily::Default,
		(constant::FONT_PATH / "default.ttf").string().c_str(),
		(constant::FONT_PATH /"default_bold.ttf").string().c_str()
	);

	FontSystem::RegisterFont(FontFamily::Fancy,
		(constant::FONT_PATH / "black_chancery.ttf").string().c_str()
	);

	FontSystem::RegisterFont(FontFamily::DialogTitle,
		(constant::FONT_PATH / "times_new_roman.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_bold.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_italic.ttf").string().c_str()
	);

	m_pSceneManager->SetScene<LoadingScene>();

	m_perodicTimer = std::thread([this]() {
		while (m_isRunning.load()) {
			PeriodicTimerEvent ev{};
			Application::OnEvent(ev);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		});
	m_perodicTimer.detach();
}

void Game::OnDetach()
{
	m_isRunning.store(false);
	if(m_perodicTimer.joinable()) {
		m_perodicTimer.join();
	}
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
		auto [mouseX, mouseY] = rlx::GetMousePosition();

		if (m_sprites.Contains(SPRID_MOUSECURSOR)) {
			m_sprites[SPRID_MOUSECURSOR]->Draw(
				(int)mouseX,
				(int)mouseY,
				SPR_MOUSECURSOR::DEFAULT
			);
		}
	}
	DrawText(FontFamily::Default, 15, std::to_string(Application::GetFPS()).c_str(), 0, 0, WHITE, FontStyle::Regular);
}

void Game::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);

	dispatcher.Dispatch<WindowFocusEvent>([this](WindowFocusEvent& e) {
		HideCursor();
		rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);
		return false;
		});

	dispatcher.Dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent& e) {
		ShowCursor();
		rlx::UnlockCursor();
		return false;
		});

	dispatcher.Dispatch<AfterUpscaleEvent>([this](AfterUpscaleEvent& e) {
		if (HardwareCursor) {
			// Draw mouse cursor
			auto [mouseX, mouseY] = GetMousePosition();
			if (m_sprites.Contains(SPRID_MOUSECURSOR)) {
				m_sprites[SPRID_MOUSECURSOR]->Draw(
					(int)mouseX,
					(int)mouseY,
					SPR_MOUSECURSOR::DEFAULT
				);
			}
		}
		return false;
		});

	dispatcher.Dispatch<PeriodicTimerEvent>([this](PeriodicTimerEvent& e) {
		for (auto& sprite : m_sprites) {
			sprite.second->UnloadIfUnused(GetTime(), 60.0f); // Unload sprites unused for 60 seconds
		}
		return false;
		});
}
