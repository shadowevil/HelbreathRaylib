#include "Sprite.h"

void CSprite::Draw(int x, int y, size_t frame)
{
	Preload();

	size_t frame_count = m_sprite_rectangles.size();
	if (frame_count == 0)
		return;

	if (frame >= frame_count)
		return;

	auto& rect = m_sprite_rectangles[frame];

	DrawTexturePro(
		m_texture,
		rlRectangle{
			static_cast<float>(rect.x),
			static_cast<float>(rect.y),
			static_cast<float>(rect.width),
			static_cast<float>(rect.height)
		},
		rlRectangle{
			static_cast<float>(x + rect.pivotX),
			static_cast<float>(y + rect.pivotY),
			static_cast<float>(rect.width),
			static_cast<float>(rect.height)
		},
		Vector2{ 0.0f, 0.0f },
		0.0f,
		WHITE
	);
}
