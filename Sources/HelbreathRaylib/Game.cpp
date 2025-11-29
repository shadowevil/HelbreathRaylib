#include "Game.h"
#include "raylib_include.h"
#include "Scenes.h"
#include "FontSystem.h"
#include "global_constants.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "GameEvents.h"
#include "SceneManager.h"

void Game::on_attach()
{
	HideCursor();
	rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);

	scene_manager = std::make_unique<SceneManager>();

	scene_manager->set_transition_config({
		SceneManager::FAST_FADE_DURATION,
		SceneManager::FAST_FADE_DURATION,
		BLACK // fadeColor
		});

	FontSystem::register_font(FontFamily::Default,
		(constant::FONT_PATH / "default.ttf").string().c_str(),
		(constant::FONT_PATH / "default_bold.ttf").string().c_str()
	);

	FontSystem::register_font(FontFamily::Fancy,
		(constant::FONT_PATH / "black_chancery.ttf").string().c_str()
	);

	FontSystem::register_font(FontFamily::DialogTitle,
		(constant::FONT_PATH / "times_new_roman.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_bold.ttf").string().c_str(),
		(constant::FONT_PATH / "times_new_roman_italic.ttf").string().c_str()
	);

	scene_manager->set_scene<LoadingScene>();

	periodic_timer = std::thread([this]() {
		while (is_running.load()) {
			PeriodicTimerEvent ev{};
			Application::on_event(ev);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		});
	periodic_timer.detach();
}

void Game::on_detach()
{
	is_running.store(false);
	if(periodic_timer.joinable()) {
		periodic_timer.join();
	}
	scene_manager.reset();
}

void Game::on_update()
{
	scene_manager->update();
}

void Game::on_render()
{
	scene_manager->render();
	// Draw mouse cursor
	if (!hardware_cursor) {
		auto [MouseX, MouseY] = rlx::get_mouse_position();

		if (sprites.contains(SPRID_MOUSECURSOR)) {
			sprites[SPRID_MOUSECURSOR]->draw(
				(int)MouseX,
				(int)MouseY,
				SPR_MOUSECURSOR::DEFAULT
			);
		}
	}
	FontSystem::draw_text(FontFamily::Default, 15, std::to_string(Application::get_fps()).c_str(), 0, 0, WHITE, FontStyle::Regular);
}

void Game::on_event(Event& event)
{
	EventDispatcher Dispatcher(event);

	Dispatcher.dispatch<WindowFocusEvent>([this](WindowFocusEvent& e) {
		HideCursor();
		rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);
		return false;
		});

	Dispatcher.dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent& e) {
		ShowCursor();
		rlx::UnlockCursor();
		return false;
		});

	Dispatcher.dispatch<AfterUpscaleEvent>([this](AfterUpscaleEvent& e) {
		if (hardware_cursor) {
			// Draw mouse cursor
			auto [MouseX, MouseY] = GetMousePosition();
			if (sprites.contains(SPRID_MOUSECURSOR)) {
				sprites[SPRID_MOUSECURSOR]->draw(
					(int)MouseX,
					(int)MouseY,
					SPR_MOUSECURSOR::DEFAULT
				);
			}
		}
		return false;
		});

	Dispatcher.dispatch<PeriodicTimerEvent>([this](PeriodicTimerEvent& e) {
		for (auto& Sprite : sprites) {
			Sprite.second->unload_if_unused(GetTime(), 60.0f); // Unload sprites unused for 60 seconds
		}
		return false;
		});
}
