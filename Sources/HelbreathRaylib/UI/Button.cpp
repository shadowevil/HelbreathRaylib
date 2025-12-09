#include "Button.h"

namespace UI {

	Button::Button(CSpriteCollection& sprites)
		: _sprites(sprites)
	{
	}

	void Button::SetSprite(int32_t sprite_id, int32_t normal_frame) {
		SpriteConfig config;
		config.sprite_id = sprite_id;
		config.normal_frame = normal_frame;
		config.hover_frame = normal_frame;
		config.pressed_frame = normal_frame;
		config.disabled_frame = normal_frame;
		_sprite_config = config;
	}

	void Button::SetSprite(const SpriteConfig& config) {
		_sprite_config = config;
	}

	void Button::SetSpriteFromBounds(int32_t sprite_id, int32_t frame) {
		if (sprite_id < 0 || !_sprites[sprite_id])
			return;

		auto sprite_rect = _sprites[sprite_id]->get_frame_rectangle(frame);
		SetSize((float)sprite_rect.width, (float)sprite_rect.height);
		SetSprite(sprite_id, frame);
	}

	void Button::SetText(const std::string& text) {
		if (!_text_config.has_value()) {
			_text_config = TextConfig{};
		}
		_text_config->text = text;
	}

	void Button::SetText(const TextConfig& config) {
		_text_config = config;
	}

	void Button::Render() {
		if (!_visible)
			return;

		// Render sprite if configured
		if (_sprite_config.has_value() && _sprite_config->sprite_id >= 0) {
			int32_t frame = GetCurrentFrame();
			if (frame >= 0) {
				_sprites[_sprite_config->sprite_id]->draw(
					(int)_bounds.x,
					(int)_bounds.y,
					frame
				);
			}
		}

		// Render text if configured
		if (_text_config.has_value() && !_text_config->text.empty()) {
			raylib::Color text_color = GetCurrentTextColor();
			FontSystem::draw_text_aligned(
				_text_config->font_index,
				_text_config->font_size,
				_text_config->text.c_str(),
				_bounds,
				text_color,
				VerticalAlign::Middle | HorizontalAlign::Center,
				_text_config->font_style
			);
		}
	}

	int32_t Button::GetCurrentFrame() const {
		if (!_sprite_config.has_value())
			return -1;

		if (!_enabled && _sprite_config->disabled_frame >= 0)
			return _sprite_config->disabled_frame;

		if (_is_pressed && _sprite_config->pressed_frame >= 0)
			return _sprite_config->pressed_frame;

		if (_is_mouse_over && _sprite_config->hover_frame >= 0)
			return _sprite_config->hover_frame;

		return _sprite_config->normal_frame;
	}

	raylib::Color Button::GetCurrentTextColor() const {
		if (!_text_config.has_value())
			return raylib::WHITE;

		if (!_enabled)
			return _text_config->disabled_color;

		if (_is_pressed)
			return _text_config->pressed_color;

		if (_is_mouse_over)
			return _text_config->hover_color;

		return _text_config->normal_color;
	}

} // namespace UI
