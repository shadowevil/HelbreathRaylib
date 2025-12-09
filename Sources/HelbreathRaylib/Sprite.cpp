#include "Sprite.h"

void CSprite::draw(int x, int y, size_t frame)
{
	//if (!this)  // or a dedicated 'is valid' flag
	//	return;
	preload();

	size_t FrameCount = _sprite_rectangles.size();
	if (FrameCount == 0)
		return;

	if (frame >= FrameCount)
		return;

	auto& Rect = _sprite_rectangles[frame];

	DrawTexturePro(
		_texture,
		raylib::Rectangle{
			static_cast<float>(Rect.x),
			static_cast<float>(Rect.y),
			static_cast<float>(Rect.width),
			static_cast<float>(Rect.height)
		},
		raylib::Rectangle{
			static_cast<float>(x + Rect.pivotX),
			static_cast<float>(y + Rect.pivotY),
			static_cast<float>(Rect.width),
			static_cast<float>(Rect.height)
		},
		raylib::Vector2{ 0.0f, 0.0f },
		0.0f,
		raylib::WHITE
	);
}

void CSprite::draw(int x, int y, size_t frame, raylib::Color tint)
{
	//if (!this)  // or a dedicated 'is valid' flag
	//	return;
	preload();

	size_t FrameCount = _sprite_rectangles.size();
	if (FrameCount == 0)
		return;

	if (frame >= FrameCount)
		return;

	auto& Rect = _sprite_rectangles[frame];

	raylib::DrawTexturePro(
		_texture,
		raylib::Rectangle{
			static_cast<float>(Rect.x),
			static_cast<float>(Rect.y),
			static_cast<float>(Rect.width),
			static_cast<float>(Rect.height)
		},
		raylib::Rectangle{
			static_cast<float>(x + Rect.pivotX),
			static_cast<float>(y + Rect.pivotY),
			static_cast<float>(Rect.width),
			static_cast<float>(Rect.height)
		},
		raylib::Vector2{ 0.0f, 0.0f },
		0.0f,
		tint
	);
}