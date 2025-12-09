#pragma once
#include "raylib_include.h"
#include "geometry_types.h"
#include <functional>

namespace UI {

	// Base control class - similar to C# Control
	class Control {
	public:
		Control() = default;
		virtual ~Control() = default;

		// Disable copy, allow move
		Control(const Control&) = delete;
		Control& operator=(const Control&) = delete;
		Control(Control&&) = default;
		Control& operator=(Control&&) = default;

		// Core control methods
		virtual void Update();
		virtual void Render() = 0; // Pure virtual - must be implemented

		// Position and size
		void SetPosition(float x, float y);
		void SetSize(float width, float height);
		void SetBounds(float x, float y, float width, float height);

		rlx::Rectangle<float> GetBounds() const { return _bounds; }
		raylib::Vector2 GetPosition() const { return { _bounds.x, _bounds.y }; }
		raylib::Vector2 GetSize() const { return { _bounds.width, _bounds.height }; }

		// Enabled/Visible state
		void SetEnabled(bool enabled) { _enabled = enabled; }
		bool IsEnabled() const { return _enabled; }

		void SetVisible(bool visible) { _visible = visible; }
		bool IsVisible() const { return _visible; }

		// Mouse interaction
		bool IsMouseOver() const { return _is_mouse_over; }
		bool IsPressed() const { return _is_pressed; }

		// Event callbacks (C# style)
		std::function<void(Control*)> OnMouseEnter;
		std::function<void(Control*)> OnMouseLeave;
		std::function<void(Control*)> OnMouseHover;
		std::function<void(Control*)> OnMouseDown;
		std::function<void(Control*)> OnMouseUp;
		std::function<void(Control*)> OnClick;

	protected:
		// Update mouse interaction state
		virtual void UpdateMouseState();

		// Helper to check if mouse is over control
		bool CheckMouseOver() const;

		// Control bounds
		rlx::Rectangle<float> _bounds{ 0, 0, 0, 0 };

		// Control state
		bool _enabled = true;
		bool _visible = true;
		bool _is_mouse_over = false;
		bool _is_pressed = false;
		bool _was_mouse_over = false;
		bool _was_pressed = false;
	};

} // namespace UI
