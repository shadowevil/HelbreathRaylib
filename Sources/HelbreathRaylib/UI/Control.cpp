#include "Control.h"
#include "../Application.h"

namespace UI {

	void Control::Update() {
		if (!_enabled || !_visible)
			return;

		UpdateMouseState();
	}

	void Control::UpdateMouseState() {
		_was_mouse_over = _is_mouse_over;
		_was_pressed = _is_pressed;

		_is_mouse_over = CheckMouseOver();
		_is_pressed = _is_mouse_over && raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT);

		// Mouse enter event
		if (_is_mouse_over && !_was_mouse_over) {
			if (OnMouseEnter)
				OnMouseEnter(this);
		}

		// Mouse leave event
		if (!_is_mouse_over && _was_mouse_over) {
			if (OnMouseLeave)
				OnMouseLeave(this);
		}

		// Mouse hover event (while over)
		if (_is_mouse_over) {
			if (OnMouseHover)
				OnMouseHover(this);
		}

		// Mouse down event
		if (_is_pressed && !_was_pressed) {
			if (OnMouseDown)
				OnMouseDown(this);
		}

		// Mouse up event
		if (!_is_pressed && _was_pressed && _is_mouse_over) {
			// Click event (mouse up while still over control)
			if (OnClick)
				OnClick(this);

			if (OnMouseUp)
				OnMouseUp(this);
		}
	}

	bool Control::CheckMouseOver() const {
		if (!_enabled || !_visible)
			return false;

		raylib::Vector2 mouse_pos = rlx::get_mouse_position();
		return _bounds.contains(mouse_pos);
	}

	void Control::SetPosition(float x, float y) {
		_bounds.x = x;
		_bounds.y = y;
	}

	void Control::SetSize(float width, float height) {
		_bounds.width = width;
		_bounds.height = height;
	}

	void Control::SetBounds(float x, float y, float width, float height) {
		_bounds.x = x;
		_bounds.y = y;
		_bounds.width = width;
		_bounds.height = height;
	}

} // namespace UI
