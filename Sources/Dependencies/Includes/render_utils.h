#pragma once
#include <cstdint>
#include <functional>
#include <tuple>
#include <algorithm>
#include "geometry_types.h"

// Note: This file assumes raylib.h has already been included
// All raylib types and functions are defined in raylib_internal namespace and wrapped in raylib namespace

namespace rlx {
	inline void BeginUpscaleRender(raylib::RenderTexture2D target, float scale = 1.0f)
	{
		raylib::BeginTextureMode(target);
		raylib::BeginMode2D(raylib::Camera2D{ .zoom = scale });
	}

	inline float GetScale(uint16_t base_x, uint16_t base_y)
	{
		float windowW = (float)raylib::GetScreenWidth();
		float windowH = (float)raylib::GetScreenHeight();
		float scale = std::min(windowW / base_x, windowH / base_y);
		return scale;
	}

	inline std::tuple<float, float, float, float> GetUpscaledTargetArea(int width, int height)
	{
		float scale = GetScale(width, height);
		float renderW = width * scale;
		float renderH = height * scale;
		float offsetX = (raylib::GetScreenWidth() - renderW) * 0.5f;
		float offsetY = (raylib::GetScreenHeight() - renderH) * 0.5f;
		return { offsetX, offsetY, renderW, renderH };
	}

	inline void EndUpscaleRender(raylib::RenderTexture2D target, raylib::Color background, std::function<void()> before = nullptr, std::function<void()> after = nullptr)
	{
		raylib::EndMode2D();
		raylib::EndTextureMode();
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(target.texture.width, target.texture.height);
		raylib::BeginDrawing();
		raylib::ClearBackground(background);
		if (before)
			before();

		raylib::DrawTexturePro(
			target.texture,
			{ 0, 0, (float)target.texture.width, -(float)target.texture.height },
			{ offsetX, offsetY, renderW, renderH },
			{ 0, 0 },
			0,
			raylib::WHITE
		);

		if (after)
			after();
		raylib::EndDrawing();
	}

	template<typename T>
	inline bool RectangleContainsScaledMouse(const rlx::Rectangle<T>& rect, uint16_t base_x, uint16_t base_y)
	{
		float scale = GetScale(base_x, base_y);
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(base_x, base_y);

		rlx::Rectangle<T> scaled_rect{
			static_cast<T>(rect.x * scale + offsetX),
			static_cast<T>(rect.y * scale + offsetY),
			static_cast<T>(rect.width * scale),
			static_cast<T>(rect.height * scale)
		};

		auto [mouseX, mouseY] = raylib::GetMousePosition();
		return scaled_rect.contains(mouseX, mouseY);
	}

	inline raylib::Vector2 GetScaledMousePosition(uint16_t base_x, uint16_t base_y)
	{
		float scale = GetScale(base_x, base_y);
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(base_x, base_y);
		auto [mouseX, mouseY] = raylib::GetMousePosition();
		float scaledX = (mouseX - offsetX) / scale;
		float scaledY = (mouseY - offsetY) / scale;

		// Clamp to valid game area (0 to base_x/base_y)
		scaledX = std::max(0.0f, std::min(scaledX, (float)base_x));
		scaledY = std::max(0.0f, std::min(scaledY, (float)base_y));

		return raylib::Vector2{ scaledX, scaledY };
	}

	inline void LockCursor(int width, int height) {
#ifdef _WIN32
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(width, height);

		windows::HWND hwnd = windows::GetActiveWindow();
		windows::RECT windowRect, clientRect;
		windows::GetWindowRect(hwnd, &windowRect);
		windows::GetClientRect(hwnd, &clientRect);

		windows::POINT clientTopLeft = { 0, 0 };
		windows::ClientToScreen(hwnd, &clientTopLeft);

		windows::LONG borderX = clientTopLeft.x - windowRect.left;
		windows::LONG borderY = clientTopLeft.y - windowRect.top;

		windows::RECT clipRect;
		clipRect.left = windowRect.left + borderX + static_cast<windows::LONG>(offsetX);
		clipRect.top = windowRect.top + borderY + static_cast<windows::LONG>(offsetY);
		clipRect.right = clipRect.left + static_cast<windows::LONG>(renderW);
		clipRect.bottom = clipRect.top + static_cast<windows::LONG>(renderH);

		windows::ClipCursor(&clipRect);
#endif
		// Note: Pointer Lock API disabled for Emscripten
		// Point-and-click games need normal mouse position tracking
	}

	inline void UnlockCursor() {
#ifdef _WIN32
		windows::ClipCursor(nullptr);
#endif
		// Note: Pointer Lock API disabled for Emscripten
	}

	inline bool HasElapsed(double timer, double interval)
	{
		return raylib::GetTime() - timer >= interval;
	}
}
