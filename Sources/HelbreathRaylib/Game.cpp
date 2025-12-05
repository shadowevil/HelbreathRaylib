#include "Game.h"
#include "raylib_include.h"
#include "Scenes.h"
#include "FontSystem.h"
#include "global_constants.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "GameEvents.h"
#include "SceneManager.h"
#include "Application.h"
#include "Platform/Desktop/DesktopCursorManager.h"

void Game::on_attach()
{
	// Get platform services from Application
	_platform = Application::get_platform();

	// Hide and lock cursor using platform services
	auto& cursor = _platform->getCursorManager();

	// Set game area for desktop cursor lock
	if (_platform->getPlatformType() == PlatformType::Desktop) {
		auto* desktopCursor = dynamic_cast<DesktopCursorManager*>(&cursor);
		if (desktopCursor) {
			desktopCursor->setGameArea(constant::BASE_WIDTH, constant::BASE_HEIGHT);
		}
	}

	cursor.hide();
	cursor.lock();

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

	// Schedule periodic timer using platform service
	auto& timer = _platform->getTimerService();
	_sprite_unload_timer = timer.scheduleRepeating([]() {
		PeriodicTimerEvent ev{};
		Application::on_event(ev);
	}, 1.0f);
}

void Game::on_detach()
{
	// Cancel periodic timer using platform service
	if (_platform && _sprite_unload_timer != 0) {
		auto& timer = _platform->getTimerService();
		timer.cancel(_sprite_unload_timer);
	}

	scene_manager.reset();
}

void Game::on_update()
{
	// Update timer service (needed for web platform frame-based timing)
	if (_platform) {
		auto& timer = _platform->getTimerService();
		timer.update(Application::get_delta_time());
	}

	// Update cursor visibility based on mouse position (web platform only)
	// Hide hardware cursor when mouse is inside game area, show when outside
	if (_platform && _platform->getPlatformType() == PlatformType::Web) {
		auto& cursor = _platform->getCursorManager();

		// Get raw window mouse position
		auto [rawMouseX, rawMouseY] = raylib::GetMousePosition();

		// Get the upscaled game area bounds
		auto [offsetX, offsetY, renderW, renderH] = rlx::GetUpscaledTargetArea(constant::BASE_WIDTH, constant::BASE_HEIGHT);

		// Check if mouse is within the upscaled game area
		bool insideGameArea = (rawMouseX >= offsetX && rawMouseX < offsetX + renderW &&
		                       rawMouseY >= offsetY && rawMouseY < offsetY + renderH);

		if (insideGameArea && cursor.isVisible()) {
			cursor.hide();
		} else if (!insideGameArea && !cursor.isVisible()) {
			cursor.show();
		}
	}

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
		if (_platform) {
			auto& cursor = _platform->getCursorManager();
			cursor.hide();
			cursor.lock();
		}
		return false;
		});

	Dispatcher.dispatch<WindowLostFocusEvent>([this](WindowLostFocusEvent& e) {
		if (_platform) {
			auto& cursor = _platform->getCursorManager();
			cursor.show();
			cursor.unlock();
		}
		return false;
		});

	Dispatcher.dispatch<AfterUpscaleEvent>([this](AfterUpscaleEvent& e) {
		if (hardware_cursor) {
			// Draw mouse cursor using raw window coordinates
			auto [offsetX, offsetY, renderW, renderH] = rlx::GetUpscaledTargetArea(constant::BASE_WIDTH, constant::BASE_HEIGHT);
			auto [MouseX, MouseY] = GetMousePosition();

			// Clamp to the upscaled game area
			float clampedX = std::max(offsetX, std::min(MouseX, offsetX + renderW));
			float clampedY = std::max(offsetY, std::min(MouseY, offsetY + renderH));

			if (sprites.contains(SPRID_MOUSECURSOR)) {
				sprites[SPRID_MOUSECURSOR]->draw(
					(int)clampedX,
					(int)clampedY,
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
