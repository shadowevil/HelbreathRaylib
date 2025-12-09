#pragma once
#include "Control.h"
#include "../FontSystem.h"
#include "../FontIDs.h"
#include <string>
#include <vector>

namespace UI {

	enum class TextBreakMode {
		Word,      // Break at word boundaries
		Character  // Break at any character
	};

	enum class TextOverflow {
		None,      // No overflow handling
		Ellipsis,  // Add "..." when text overflows
		Clip       // Just clip the text
	};

	// Label control for displaying text with auto-sizing or manual sizing
	class Label : public Control {
	public:
		struct Config {
			std::string text;
			uint8_t font_index = 0;
			int font_size = 13;
			raylib::Color color = raylib::WHITE;
			FontStyle font_style = FontStyle::Regular;
			HorizontalAlign h_align = HorizontalAlign::Left;
			VerticalAlign v_align = VerticalAlign::Top;
			float line_spacing = 0.0f; // Additional spacing between lines
			bool auto_size = true; // If true, bounds are calculated from text
			bool word_wrap = false; // If true and auto_size is false, text wraps within bounds
			TextBreakMode break_mode = TextBreakMode::Word; // How to break lines
			bool use_hyphenation = false; // Add hyphens when breaking words
			TextOverflow overflow_horizontal = TextOverflow::None; // Horizontal overflow handling
			TextOverflow overflow_vertical = TextOverflow::None; // Vertical overflow handling
		};

		Label();
		virtual ~Label() = default;

		// Set label configuration
		void SetConfig(const Config& config);

		// Quick setters
		void SetText(const std::string& text);
		void SetColor(raylib::Color color);
		void SetFontSize(int size);
		void SetAutoSize(bool auto_size);
		void SetWordWrap(bool word_wrap);
		void SetLineSpacing(float spacing);
		void SetBreakMode(TextBreakMode mode);
		void SetHyphenation(bool use_hyphenation);
		void SetOverflowHorizontal(TextOverflow overflow);
		void SetOverflowVertical(TextOverflow overflow);

		// Get current config
		const Config& GetConfig() const { return _config; }

		// Rendering
		void Render() override;
		void Update() override; // Labels don't need updates but override for consistency

	private:
		Config _config;
		std::vector<std::string> _wrapped_lines; // Cached wrapped lines
		bool _needs_recalculate = true;
		bool _has_overflow = false; // Track if text overflows

		// Recalculate bounds and wrapped lines based on config
		void RecalculateBounds();

		// Wrap text to fit within bounds
		void WrapText();

		// Wrap text with word breaking
		void WrapTextByWord();

		// Wrap text with character breaking
		void WrapTextByCharacter();

		// Apply ellipsis to lines that overflow
		void ApplyEllipsis();

		// Measure text width using the configured font
		float MeasureText(const std::string& text) const;

		// Check if a character can break a word (for hyphenation)
		bool CanHyphenate(char c) const;
	};

} // namespace UI
