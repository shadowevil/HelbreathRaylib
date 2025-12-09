#include "InputBox.h"
#include "../Application.h"
#include <algorithm>
#include <cstring>

namespace UI {

	// Static member initialization
	InputBox* InputBox::_active_input_box = nullptr;

	InputBox::InputBox() {
		// Set default bounds
		SetBounds(0, 0, 200, 30);
	}

	InputBox::InputBox(float x, float y, float width, float height) {
		SetBounds(x, y, width, height);
	}

	InputBox::InputBox(const rlx::Rectangle<float>& bounds) {
		SetBounds(bounds.x, bounds.y, bounds.width, bounds.height);
	}

	InputBox::InputBox(const raylib::Rectangle& bounds) {
		SetBounds(bounds.x, bounds.y, bounds.width, bounds.height);
	}

	InputBox::~InputBox() {
		// If this was the active input box, clear it
		if (_active_input_box == this) {
			_active_input_box = nullptr;
		}
	}

	void InputBox::SetConfig(const Config& config) {
		_config = config;
	}

	void InputBox::SetText(const std::string& text) {
		// Respect max length
		if (text.length() > static_cast<size_t>(_config.max_length)) {
			_text = text.substr(0, _config.max_length);
		}
		else {
			_text = text;
		}

		// Clamp cursor position
		_cursor_position = std::min(_cursor_position, static_cast<int>(_text.length()));
		ClearSelection();

		if (OnTextChanged) {
			OnTextChanged(this);
		}
	}

	void InputBox::Clear() {
		_text.clear();
		_cursor_position = 0;
		ClearSelection();
		_scroll_offset = 0.0f;

		if (OnTextChanged) {
			OnTextChanged(this);
		}
	}

	void InputBox::SetMaxLength(int16_t max_length) {
		_config.max_length = max_length;

		// Truncate text if needed
		if (_text.length() > static_cast<size_t>(max_length)) {
			_text = _text.substr(0, max_length);
			_cursor_position = std::min(_cursor_position, static_cast<int>(_text.length()));
			ClearSelection();

			if (OnTextChanged) {
				OnTextChanged(this);
			}
		}
	}

	void InputBox::SetPlaceholder(const std::string& placeholder) {
		_config.placeholder_text = placeholder;
	}

	void InputBox::SetPasswordMode(bool enabled) {
		_config.password_mode = enabled;
	}

	void InputBox::SetAllowedCharacters(const std::string& allowed_chars) {
		_allowed_characters = allowed_chars;
		_has_character_filter = !allowed_chars.empty();
	}

	void InputBox::ClearAllowedCharacters() {
		_allowed_characters.clear();
		_has_character_filter = false;
	}

	bool InputBox::IsCharacterAllowed(char c) const {
		if (!_has_character_filter) return true;
		return _allowed_characters.find(c) != std::string::npos;
	}

	void InputBox::Focus() {
		if (!_is_focused) {
			// Blur any other active input box
			if (_active_input_box && _active_input_box != this) {
				_active_input_box->Blur();
			}

			_is_focused = true;
			_active_input_box = this;
			_cursor_blink_timer = 0.0f;
			_cursor_visible = true;

			if (OnFocused) {
				OnFocused(this);
			}
		}
	}

	void InputBox::Blur() {
		if (_is_focused) {
			_is_focused = false;
			if (_active_input_box == this) {
				_active_input_box = nullptr;
			}
			ClearSelection();

			if (OnBlurred) {
				OnBlurred(this);
			}
		}
	}

	void InputBox::Update() {
		if (!_enabled || !_visible)
			return;

		// Update base control (mouse state)
		Control::Update();

		// Handle mouse click for focus
		HandleMouseInput();

		// Only process keyboard input if focused
		if (_is_focused) {
			// Track key states and fire key down/up events
			for (int key = 0; key < 512; ++key) {
				bool is_down = raylib::IsKeyDown(key);

				// Key down event
				if (is_down && !_key_states[key]) {
					_key_states[key] = true;
					if (OnKeyDown) {
						OnKeyDown(this, key);
					}
				}
				// Key up event
				else if (!is_down && _key_states[key]) {
					_key_states[key] = false;
					if (OnKeyUp) {
						OnKeyUp(this, key);
					}
				}
			}

			HandleKeyboardInput();
			HandleTextInput();

			// Update cursor blink
			_cursor_blink_timer += raylib::GetFrameTime();
			if (_cursor_blink_timer >= _config.cursor_blink_rate) {
				_cursor_blink_timer = 0.0f;
				_cursor_visible = !_cursor_visible;
			}

			// Update scroll offset to keep cursor visible
			UpdateScrollOffset();
		}
		else {
			// Clear key states when not focused
			std::memset(_key_states, 0, sizeof(_key_states));
		}
	}

	void InputBox::Render() {
		if (!_visible)
			return;

		// Draw background
		raylib::DrawRectangleRec(_bounds, _config.background_color);

		// Draw border
		raylib::Color border_color = _is_focused ? _config.focused_border_color : _config.border_color;
		raylib::DrawRectangleLinesEx(_bounds, _config.border_thickness, border_color);

		// Get text bounds for rendering (this is the clipping area)
		raylib::Rectangle text_bounds = GetTextBounds();

		// Ensure text bounds are valid for scissor mode
		if (text_bounds.width <= 0 || text_bounds.height <= 0) {
			return;
		}

		// Use scissor stack for proper nested clipping with OpenGL
		// This allows multiple UI elements to have their own clipping regions
		ScissorStack::Push(text_bounds);

		std::string display_text = GetDisplayText();
		raylib::Font& font = FontSystem::get_font(_config.font_index, _config.font_size, _config.font_style);

		// Calculate text position with scroll offset applied
		float text_x = text_bounds.x - _scroll_offset;
		float text_y = text_bounds.y + (text_bounds.height) / 2.0f - _config.font_size / 2.0f;

		// Draw selection background if there's a selection
		if (_is_focused && HasSelection()) {
			int sel_min = GetSelectionMin();
			int sel_max = GetSelectionMax();

			std::string before_sel = _config.password_mode
				? std::string(sel_min, _config.password_char)
				: _text.substr(0, sel_min);

			std::string selected = _config.password_mode
				? std::string(sel_max - sel_min, _config.password_char)
				: _text.substr(sel_min, sel_max - sel_min);

			raylib::Vector2 before_size = raylib::MeasureTextEx(font, before_sel.c_str(),
				static_cast<float>(_config.font_size), 0.0f);
			raylib::Vector2 sel_size = raylib::MeasureTextEx(font, selected.c_str(),
				static_cast<float>(_config.font_size), 0.0f);

			float sel_x = text_x + before_size.x;
			float sel_width = sel_size.x;

			// Clamp selection rendering to visible bounds
			if (sel_x < text_bounds.x + text_bounds.width && sel_x + sel_width > text_bounds.x) {
				raylib::DrawRectangle(
					static_cast<int>(sel_x),
					static_cast<int>(text_bounds.y),
					static_cast<int>(sel_width),
					static_cast<int>(text_bounds.height),
					_config.selection_color
				);
			}
		}

		// Draw text or placeholder
		if (_text.empty() && !_config.placeholder_text.empty() && !_is_focused) {
			FontSystem::draw_text(_config.font_index, _config.font_size, _config.placeholder_text.c_str(),
				text_x, text_y, _config.placeholder_color, _config.font_style | FontStyle::Shadow);
		}
		else if (!display_text.empty()) {
			FontSystem::draw_text(_config.font_index, _config.font_size, display_text.c_str(),
				text_x, text_y, _config.text_color, _config.font_style | FontStyle::Shadow);
		}

		// Draw cursor (only when visible and in bounds)
		if (_is_focused && _cursor_visible) {
			int cursor_pixel_pos = GetCursorPixelPosition();
			float cursor_x = text_x + cursor_pixel_pos;

			// Only draw cursor if it's within the visible bounds
			if (cursor_x >= text_bounds.x && cursor_x < text_bounds.x + text_bounds.width) {
				raylib::DrawRectangle(
					static_cast<int>(cursor_x),
					static_cast<int>(text_bounds.y + 2),
					static_cast<int>(_config.cursor_width),
					static_cast<int>(text_bounds.height - 4),
					_config.cursor_color
				);
			}
		}

		// Pop scissor region - restores previous scissor state or disables if stack is empty
		ScissorStack::Pop();
	}

	void InputBox::HandleMouseInput() {
		// Check for focus on click
		if (_is_mouse_over && raylib::IsMouseButtonPressed(raylib::MOUSE_BUTTON_LEFT)) {
			Focus();

			// Position cursor at click location
			raylib::Vector2 mouse_pos = rlx::get_mouse_position();
			raylib::Rectangle text_bounds = GetTextBounds();
			float relative_x = mouse_pos.x - text_bounds.x + _scroll_offset;

			int char_pos = GetCharacterAtPosition(relative_x);
			MoveCursor(char_pos, false);
		}
		// Handle drag selection
		else if (_is_focused && raylib::IsMouseButtonDown(raylib::MOUSE_BUTTON_LEFT) && _is_mouse_over) {
			raylib::Vector2 mouse_pos = rlx::get_mouse_position();
			raylib::Rectangle text_bounds = GetTextBounds();
			float relative_x = mouse_pos.x - text_bounds.x + _scroll_offset;

			int char_pos = GetCharacterAtPosition(relative_x);
			MoveCursor(char_pos, true);
		}
		// Lose focus on click outside
		else if (_is_focused && !_is_mouse_over && raylib::IsMouseButtonPressed(raylib::MOUSE_BUTTON_LEFT)) {
			Blur();
		}
	}

	void InputBox::HandleKeyboardInput() {
		// Handle special keys
		HandleCursorMovement();
		HandleCopyPaste();

		// Enter key
		if (raylib::IsKeyPressed(raylib::KEY_ENTER) || raylib::IsKeyPressed(raylib::KEY_KP_ENTER)) {
			if (OnEnterPressed) {
				OnEnterPressed(this);
			}
		}

		// Backspace
		if (raylib::IsKeyPressed(raylib::KEY_BACKSPACE) ||
			(raylib::IsKeyDown(raylib::KEY_BACKSPACE) && raylib::IsKeyPressedRepeat(raylib::KEY_BACKSPACE))) {
			if (HasSelection()) {
				DeleteSelection();
			}
			else {
				DeleteCharacterBeforeCursor();
			}
		}

		// Delete
		if (raylib::IsKeyPressed(raylib::KEY_DELETE) ||
			(raylib::IsKeyDown(raylib::KEY_DELETE) && raylib::IsKeyPressedRepeat(raylib::KEY_DELETE))) {
			if (HasSelection()) {
				DeleteSelection();
			}
			else {
				DeleteCharacterAtCursor();
			}
		}

		// Escape to blur
		if (raylib::IsKeyPressed(raylib::KEY_ESCAPE)) {
			Blur();
		}
	}

	void InputBox::HandleTextInput() {
		int key = raylib::GetCharPressed();
		while (key > 0) {
			// Only accept printable characters
			if (key >= 32 && key < 127) {
				char c = static_cast<char>(key);

				// Fire character input event
				if (OnCharInput) {
					OnCharInput(this, c);
				}

				// Check if character is allowed
				if (IsCharacterAllowed(c)) {
					if (HasSelection()) {
						DeleteSelection();
					}

					InsertText(std::string(1, c));
				}
			}
			key = raylib::GetCharPressed();
		}
	}

	void InputBox::HandleCursorMovement() {
		bool shift_held = raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT) || raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT);
		bool ctrl_held = raylib::IsKeyDown(raylib::KEY_LEFT_CONTROL) || raylib::IsKeyDown(raylib::KEY_RIGHT_CONTROL);

		// Left arrow
		if (raylib::IsKeyPressed(raylib::KEY_LEFT) ||
			(raylib::IsKeyDown(raylib::KEY_LEFT) && raylib::IsKeyPressedRepeat(raylib::KEY_LEFT))) {
			if (ctrl_held) {
				// Ctrl+Left: Move to start
				MoveCursorToStart(shift_held);
			}
			else {
				MoveCursorLeft(shift_held);
			}
		}

		// Right arrow
		if (raylib::IsKeyPressed(raylib::KEY_RIGHT) ||
			(raylib::IsKeyDown(raylib::KEY_RIGHT) && raylib::IsKeyPressedRepeat(raylib::KEY_RIGHT))) {
			if (ctrl_held) {
				// Ctrl+Right: Move to end
				MoveCursorToEnd(shift_held);
			}
			else {
				MoveCursorRight(shift_held);
			}
		}

		// Home
		if (raylib::IsKeyPressed(raylib::KEY_HOME)) {
			MoveCursorToStart(shift_held);
		}

		// End
		if (raylib::IsKeyPressed(raylib::KEY_END)) {
			MoveCursorToEnd(shift_held);
		}
	}

	void InputBox::HandleCopyPaste() {
		bool ctrl_held = raylib::IsKeyDown(raylib::KEY_LEFT_CONTROL) || raylib::IsKeyDown(raylib::KEY_RIGHT_CONTROL);

		if (ctrl_held) {
			// Ctrl+A: Select all
			if (raylib::IsKeyPressed(raylib::KEY_A)) {
				SelectAll();
			}

			// Ctrl+C: Copy
			if (raylib::IsKeyPressed(raylib::KEY_C)) {
				if (HasSelection()) {
					std::string selected_text = GetSelectedText();
					CopyToClipboard(selected_text);
				}
			}

			// Ctrl+X: Cut
			if (raylib::IsKeyPressed(raylib::KEY_X)) {
				if (HasSelection()) {
					std::string selected_text = GetSelectedText();
					CopyToClipboard(selected_text);
					DeleteSelection();
				}
			}

			// Ctrl+V: Paste
			if (raylib::IsKeyPressed(raylib::KEY_V)) {
				std::string clipboard_text = GetFromClipboard();
				if (!clipboard_text.empty()) {
					if (HasSelection()) {
						DeleteSelection();
					}
					InsertText(clipboard_text);
				}
			}
		}
	}

	void InputBox::CopyToClipboard(const std::string& text) {
		raylib::SetClipboardText(text.c_str());
	}

	std::string InputBox::GetFromClipboard() {
		const char* clipboard = raylib::GetClipboardText();
		return clipboard ? std::string(clipboard) : "";
	}

	void InputBox::ClearSelection() {
		_selection_start = -1;
		_selection_end = -1;
	}

	void InputBox::SelectAll() {
		_selection_start = 0;
		_selection_end = static_cast<int>(_text.length());
	}

	std::string InputBox::GetSelectedText() const {
		if (!HasSelection()) return "";
		int min_pos = GetSelectionMin();
		int max_pos = GetSelectionMax();
		return _text.substr(min_pos, max_pos - min_pos);
	}

	void InputBox::DeleteSelection() {
		if (!HasSelection()) return;

		int min_pos = GetSelectionMin();
		int max_pos = GetSelectionMax();

		_text.erase(min_pos, max_pos - min_pos);
		_cursor_position = min_pos;
		ClearSelection();

		if (OnTextChanged) {
			OnTextChanged(this);
		}
	}

	void InputBox::MoveCursor(int position, bool select) {
		position = std::clamp(position, 0, static_cast<int>(_text.length()));

		if (select) {
			// Start selection if not already selecting
			if (!HasSelection()) {
				_selection_start = _cursor_position;
			}
			_selection_end = position;
		}
		else {
			ClearSelection();
		}

		_cursor_position = position;
		_cursor_blink_timer = 0.0f;
		_cursor_visible = true;
	}

	void InputBox::MoveCursorLeft(bool select) {
		if (!select && HasSelection()) {
			// Just move to start of selection
			_cursor_position = GetSelectionMin();
			ClearSelection();
		}
		else if (_cursor_position > 0) {
			MoveCursor(_cursor_position - 1, select);
		}
	}

	void InputBox::MoveCursorRight(bool select) {
		if (!select && HasSelection()) {
			// Just move to end of selection
			_cursor_position = GetSelectionMax();
			ClearSelection();
		}
		else if (_cursor_position < static_cast<int>(_text.length())) {
			MoveCursor(_cursor_position + 1, select);
		}
	}

	void InputBox::MoveCursorToStart(bool select) {
		MoveCursor(0, select);
	}

	void InputBox::MoveCursorToEnd(bool select) {
		MoveCursor(static_cast<int>(_text.length()), select);
	}

	int InputBox::GetCursorPixelPosition() const {
		std::string text_before_cursor = _config.password_mode
			? std::string(_cursor_position, _config.password_char)
			: _text.substr(0, _cursor_position);

		raylib::Font& font = FontSystem::get_font(_config.font_index, _config.font_size, _config.font_style);
		raylib::Vector2 size = raylib::MeasureTextEx(font, text_before_cursor.c_str(),
			static_cast<float>(_config.font_size), 0.0f);

		return static_cast<int>(size.x);
	}

	int InputBox::GetCharacterAtPosition(float pixel_x) const {
		raylib::Font& font = FontSystem::get_font(_config.font_index, _config.font_size, _config.font_style);

		for (size_t i = 0; i <= _text.length(); ++i) {
			std::string text_slice = _config.password_mode
				? std::string(i, _config.password_char)
				: _text.substr(0, i);

			raylib::Vector2 size = raylib::MeasureTextEx(font, text_slice.c_str(),
				static_cast<float>(_config.font_size), 0.0f);

			if (size.x >= pixel_x) {
				// Check if closer to previous character
				if (i > 0) {
					std::string prev_slice = _config.password_mode
						? std::string(i - 1, _config.password_char)
						: _text.substr(0, i - 1);
					raylib::Vector2 prev_size = raylib::MeasureTextEx(font, prev_slice.c_str(),
						static_cast<float>(_config.font_size), 0.0f);

					float dist_to_current = size.x - pixel_x;
					float dist_to_prev = pixel_x - prev_size.x;

					if (dist_to_prev < dist_to_current) {
						return static_cast<int>(i - 1);
					}
				}
				return static_cast<int>(i);
			}
		}

		return static_cast<int>(_text.length());
	}

	void InputBox::InsertText(const std::string& text) {
		// Check if we have room
		int available_space = _config.max_length - static_cast<int>(_text.length());
		if (available_space <= 0) return;

		// Truncate if necessary
		std::string to_insert = text.length() > static_cast<size_t>(available_space)
			? text.substr(0, available_space)
			: text;

		_text.insert(_cursor_position, to_insert);
		_cursor_position += static_cast<int>(to_insert.length());
		_cursor_blink_timer = 0.0f;
		_cursor_visible = true;

		if (OnTextChanged) {
			OnTextChanged(this);
		}
	}

	void InputBox::DeleteCharacterAtCursor() {
		if (_cursor_position < static_cast<int>(_text.length())) {
			_text.erase(_cursor_position, 1);

			if (OnTextChanged) {
				OnTextChanged(this);
			}
		}
	}

	void InputBox::DeleteCharacterBeforeCursor() {
		if (_cursor_position > 0) {
			_text.erase(_cursor_position - 1, 1);
			_cursor_position--;
			_cursor_blink_timer = 0.0f;
			_cursor_visible = true;

			if (OnTextChanged) {
				OnTextChanged(this);
			}
		}
	}

	std::string InputBox::GetDisplayText() const {
		if (_config.password_mode && !_text.empty()) {
			return std::string(_text.length(), _config.password_char);
		}
		return _text;
	}

	void InputBox::UpdateScrollOffset() {
		EnsureCursorVisible();
	}

	void InputBox::EnsureCursorVisible() {
		raylib::Rectangle text_bounds = GetTextBounds();
		if (text_bounds.width <= 0) return;

		int cursor_pixel_pos = GetCursorPixelPosition();

		// Calculate cursor position relative to the visible area
		float cursor_screen_pos = cursor_pixel_pos - _scroll_offset;

		// Small margin to keep cursor comfortably visible
		const float margin = 5.0f;

		// Scroll right if cursor is off right edge
		if (cursor_screen_pos > text_bounds.width - margin) {
			_scroll_offset = cursor_pixel_pos - text_bounds.width + margin;
		}
		// Scroll left if cursor is off left edge
		else if (cursor_screen_pos < margin) {
			_scroll_offset = cursor_pixel_pos - margin;
		}

		// Clamp scroll offset to valid range
		float max_scroll = GetMaxScrollOffset();
		_scroll_offset = std::clamp(_scroll_offset, 0.0f, max_scroll);
	}

	float InputBox::GetMaxScrollOffset() const {
		raylib::Rectangle text_bounds = GetTextBounds();
		if (text_bounds.width <= 0) return 0.0f;

		// Calculate total text width
		std::string display_text = GetDisplayText();
		raylib::Font& font = FontSystem::get_font(_config.font_index, _config.font_size, _config.font_style);
		raylib::Vector2 text_size = raylib::MeasureTextEx(font, display_text.c_str(),
			static_cast<float>(_config.font_size), 0.0f);

		// Maximum scroll is when the end of text aligns with the right edge
		float max_scroll = text_size.x - text_bounds.width;
		return std::max(0.0f, max_scroll);
	}

	void InputBox::ScrollToStart() {
		_scroll_offset = 0.0f;
	}

	void InputBox::ScrollToEnd() {
		_scroll_offset = GetMaxScrollOffset();
	}

	void InputBox::ScrollToCursor() {
		EnsureCursorVisible();
	}

	raylib::Rectangle InputBox::GetTextBounds() const {
		return raylib::Rectangle{
			_bounds.x + _config.padding_left + _config.border_thickness,
			_bounds.y + _config.padding_top + _config.border_thickness,
			_bounds.width - _config.padding_left - _config.padding_right - _config.border_thickness * 2,
			_bounds.height - _config.padding_top - _config.padding_bottom - _config.border_thickness * 2
		};
	}

} // namespace UI
