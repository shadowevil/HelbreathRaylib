#include "ToggleButton.h"

namespace UI {

	ToggleButton::ToggleButton(CSpriteCollection& sprites)
		: Button(sprites)
	{
		// Override OnClick to handle toggle
		OnClick = [this](Control*) {
			Toggle();
			};
	}

	void ToggleButton::SetToggled(bool toggled) {
		if (_is_toggled == toggled)
			return;

		_is_toggled = toggled;

		if (OnToggleChanged)
			OnToggleChanged(this, _is_toggled);
	}

	void ToggleButton::Toggle() {
		SetToggled(!_is_toggled);
	}

	void ToggleButton::Update() {
		// Call base update
		Button::Update();
	}

	int32_t ToggleButton::GetCurrentFrame() const {
		if (!_sprite_config.has_value())
			return -1;

		if (!_enabled && _sprite_config->disabled_frame >= 0)
			return _sprite_config->disabled_frame;

		// When toggled, show hover frame to indicate selection
		if (_is_toggled && _sprite_config->hover_frame >= 0)
			return _sprite_config->hover_frame;

		if (_is_pressed && _sprite_config->pressed_frame >= 0)
			return _sprite_config->pressed_frame;

		if (_is_mouse_over && _sprite_config->hover_frame >= 0)
			return _sprite_config->hover_frame;

		return _sprite_config->normal_frame;
	}

	raylib::Color ToggleButton::GetCurrentTextColor() const {
		if (!_text_config.has_value())
			return raylib::WHITE;

		if (!_enabled)
			return _text_config->disabled_color;

		// When toggled, show hover color to indicate selection
		if (_is_toggled)
			return _text_config->hover_color;

		if (_is_pressed)
			return _text_config->pressed_color;

		if (_is_mouse_over)
			return _text_config->hover_color;

		return _text_config->normal_color;
	}

} // namespace UI
