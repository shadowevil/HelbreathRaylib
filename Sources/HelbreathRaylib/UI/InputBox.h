#pragma once
#include "Control.h"
#include "ScissorStack.h"
#include "../FontSystem.h"
#include <string>
#include <optional>
#include <cstdint>

namespace UI
{

	class InputBox : public Control
	{
	public:
		struct Config
		{
			// Visual
			raylib::Color background_color = raylib::Color{30, 30, 30, 0};
			raylib::Color border_color = raylib::Color{100, 100, 100, 0};
			raylib::Color focused_border_color = raylib::Color{200, 200, 200, 0};
			raylib::Color text_color = raylib::WHITE;
			raylib::Color placeholder_color = raylib::Color{200, 200, 200, 255};
			raylib::Color selection_color = raylib::Color{51, 153, 255, 128};
			raylib::Color cursor_color = raylib::Color{232, 232, 232, 255};

			// Font
			uint8_t font_index = 0;
			int font_size = 13;
			FontStyle font_style = FontStyle::Regular;

			// Behavior
			int16_t max_length = 32767;
			std::string placeholder_text = "";
			bool password_mode = false;
			char password_char = '*';

			// Padding
			float padding_left = 2.0f;
			float padding_right = 2.0f;
			float padding_top = 1.0f;
			float padding_bottom = 1.0f;

			// Border
			float border_thickness = 0.0f;

			// Cursor
			float cursor_blink_rate = 0.5f; // seconds
			float cursor_width = 1.0f;
		};

		InputBox();
		InputBox(float x, float y, float width, float height);
		InputBox(const rlx::Rectangle<float> &bounds);
		InputBox(const raylib::Rectangle &bounds);
		virtual ~InputBox();

		// Configuration
		void SetConfig(const Config &config);
		const Config &GetConfig() const { return _config; }

		// Text management
		void SetText(const std::string &text);
		std::string GetText() const { return _text; }
		void Clear();

		// Max length
		void SetMaxLength(int16_t max_length);
		int16_t GetMaxLength() const { return _config.max_length; }

		// Placeholder
		void SetPlaceholder(const std::string &placeholder);

		// Password mode
		void SetPasswordMode(bool enabled);
		bool IsPasswordMode() const { return _config.password_mode; }

		// Focus management
		void Focus();
		void Blur();
		bool IsFocused() const { return _is_focused; }

		// Static accessor for active input box
		static InputBox *GetActiveInputBox() { return _active_input_box; }

		// Override base methods
		void Update() override;
		void Render() override;

		// Character filtering
		void SetAllowedCharacters(const std::string &allowed_chars);
		void ClearAllowedCharacters();
		bool IsCharacterAllowed(char c) const;

		// Scroll management
		float GetMaxScrollOffset() const;
		void ScrollToStart();
		void ScrollToEnd();
		void ScrollToCursor();

		// Events
		std::function<void(InputBox *)> OnTextChanged;
		std::function<void(InputBox *)> OnFocused;
		std::function<void(InputBox *)> OnBlurred;
		std::function<void(InputBox *)> OnEnterPressed;
		std::function<void(InputBox *, int)> OnKeyDown;	   // Called with key code
		std::function<void(InputBox *, int)> OnKeyUp;	   // Called with key code
		std::function<void(InputBox *, char)> OnCharInput; // Called with character input

	private:
		// Text and cursor
		std::string _text;
		int _cursor_position = 0;
		int _selection_start = -1; // -1 means no selection
		int _selection_end = -1;

		// Visual state
		Config _config;
		bool _is_focused = false;
		float _cursor_blink_timer = 0.0f;
		bool _cursor_visible = true;
		float _scroll_offset = 0.0f; // Horizontal scroll for long text

		// Static active input box tracker
		static InputBox *_active_input_box;

		// Character filtering
		std::string _allowed_characters;
		bool _has_character_filter = false;

		// Key tracking for events
		bool _key_states[512] = {false}; // Track key down states

		// Input handling
		void HandleKeyboardInput();
		void HandleMouseInput();
		void HandleTextInput();
		void HandleCursorMovement();
		void HandleCopyPaste();

		// Clipboard (cross-platform)
		void CopyToClipboard(const std::string &text);
		std::string GetFromClipboard();

		// Selection helpers
		bool HasSelection() const { return _selection_start >= 0 && _selection_end >= 0 && _selection_start != _selection_end; }
		void ClearSelection();
		void SelectAll();
		std::string GetSelectedText() const;
		void DeleteSelection();
		int GetSelectionMin() const { return std::min(_selection_start, _selection_end); }
		int GetSelectionMax() const { return std::max(_selection_start, _selection_end); }

		// Cursor helpers
		void MoveCursor(int position, bool select = false);
		void MoveCursorLeft(bool select = false);
		void MoveCursorRight(bool select = false);
		void MoveCursorToStart(bool select = false);
		void MoveCursorToEnd(bool select = false);
		int GetCursorPixelPosition() const;
		int GetCharacterAtPosition(float pixel_x) const;

		// Text manipulation
		void InsertText(const std::string &text);
		void DeleteCharacterAtCursor();
		void DeleteCharacterBeforeCursor();

		// Rendering helpers
		std::string GetDisplayText() const;
		void UpdateScrollOffset();
		void EnsureCursorVisible();
		raylib::Rectangle GetTextBounds() const;
	};

} // namespace UI
