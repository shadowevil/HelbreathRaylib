#include "Label.h"
#include <sstream>
#include <algorithm>

namespace UI {

	Label::Label()
	{
		_enabled = false; // Labels don't respond to input
	}

	void Label::SetConfig(const Config& config) {
		_config = config;
		_needs_recalculate = true;
	}

	void Label::SetText(const std::string& text) {
		if (_config.text != text) {
			_config.text = text;
			_needs_recalculate = true;
		}
	}

	void Label::SetColor(raylib::Color color) {
		_config.color = color;
	}

	void Label::SetFontSize(int size) {
		if (_config.font_size != size) {
			_config.font_size = size;
			_needs_recalculate = true;
		}
	}

	void Label::SetAutoSize(bool auto_size) {
		if (_config.auto_size != auto_size) {
			_config.auto_size = auto_size;
			_needs_recalculate = true;
		}
	}

	void Label::SetWordWrap(bool word_wrap) {
		if (_config.word_wrap != word_wrap) {
			_config.word_wrap = word_wrap;
			_needs_recalculate = true;
		}
	}

	void Label::SetLineSpacing(float spacing) {
		if (_config.line_spacing != spacing) {
			_config.line_spacing = spacing;
			_needs_recalculate = true;
		}
	}

	void Label::SetBreakMode(TextBreakMode mode) {
		if (_config.break_mode != mode) {
			_config.break_mode = mode;
			_needs_recalculate = true;
		}
	}

	void Label::SetHyphenation(bool use_hyphenation) {
		if (_config.use_hyphenation != use_hyphenation) {
			_config.use_hyphenation = use_hyphenation;
			_needs_recalculate = true;
		}
	}

	void Label::SetOverflowHorizontal(TextOverflow overflow) {
		if (_config.overflow_horizontal != overflow) {
			_config.overflow_horizontal = overflow;
			_needs_recalculate = true;
		}
	}

	void Label::SetOverflowVertical(TextOverflow overflow) {
		if (_config.overflow_vertical != overflow) {
			_config.overflow_vertical = overflow;
			_needs_recalculate = true;
		}
	}

	void Label::Update() {
		// Labels are passive and don't need updates
		// This override exists for consistency with other controls
	}

	void Label::Render() {
		if (!_visible || _config.text.empty())
			return;

		if (_needs_recalculate) {
			RecalculateBounds();
			_needs_recalculate = false;
		}

		float current_y = _bounds.y;
		float line_height = static_cast<float>(_config.font_size) + _config.line_spacing;

		// Apply vertical alignment for the entire text block
		if (!_config.auto_size) {
			float total_height = static_cast<float>(_wrapped_lines.size()) * line_height;

			if (_config.v_align == VerticalAlign::Middle) {
				current_y = _bounds.y + (_bounds.height - total_height) / 2.0f;
			}
			else if (_config.v_align == VerticalAlign::Bottom) {
				current_y = _bounds.y + _bounds.height - total_height;
			}
		}

		// Render each line
		for (const auto& line : _wrapped_lines) {
			// Check vertical overflow
			if (!_config.auto_size && current_y + line_height > _bounds.y + _bounds.height) {
				break; // Stop rendering lines that exceed vertical bounds
			}

			if (line.empty()) {
				current_y += line_height;
				continue;
			}

			// Calculate line bounds for alignment
			rlx::Rectangle<float> line_bounds = { _bounds.x, current_y, _bounds.width, line_height };

			// Draw the line with per-line alignment
			TextAlign alignment = static_cast<TextAlign>(
				static_cast<uint8_t>(_config.v_align) | static_cast<uint8_t>(_config.h_align)
			);

			FontSystem::draw_text_aligned(
				_config.font_index,
				_config.font_size,
				line.c_str(),
				line_bounds,
				_config.color,
				alignment,
				_config.font_style
			);

			current_y += line_height;
		}
	}

	void Label::RecalculateBounds() {
		// Split text into lines and handle word wrapping if needed
		if (_config.word_wrap && !_config.auto_size) {
			WrapText();
		}
		else {
			// Simple newline split
			_wrapped_lines.clear();
			std::istringstream stream(_config.text);
			std::string line;
			while (std::getline(stream, line)) {
				_wrapped_lines.push_back(line);
			}
		}

		// Apply ellipsis if needed
		if (!_config.auto_size &&
			(_config.overflow_horizontal != TextOverflow::None || _config.overflow_vertical != TextOverflow::None)) {
			ApplyEllipsis();
		}

		// If auto-size, calculate bounds from text
		if (_config.auto_size) {
			float max_width = 0.0f;

			for (const auto& line : _wrapped_lines) {
				if (!line.empty()) {
					max_width = std::max(max_width, MeasureText(line));
				}
			}

			float line_height = static_cast<float>(_config.font_size) + _config.line_spacing;
			float total_height = static_cast<float>(_wrapped_lines.size()) * line_height;

			_bounds.width = max_width;
			_bounds.height = total_height;
		}
	}

	void Label::WrapText() {
		if (_config.break_mode == TextBreakMode::Word) {
			WrapTextByWord();
		}
		else {
			WrapTextByCharacter();
		}
	}

	void Label::WrapTextByWord() {
		_wrapped_lines.clear();

		if (_bounds.width <= 0.0f) {
			// No width to wrap to, just split by newlines
			std::istringstream stream(_config.text);
			std::string line;
			while (std::getline(stream, line)) {
				_wrapped_lines.push_back(line);
			}
			return;
		}

		// Split by newlines first
		std::istringstream stream(_config.text);
		std::string paragraph;

		while (std::getline(stream, paragraph)) {
			if (paragraph.empty()) {
				_wrapped_lines.push_back("");
				continue;
			}

			// Word wrap this paragraph
			std::istringstream word_stream(paragraph);
			std::string word;
			std::string current_line;

			while (word_stream >> word) {
				std::string test_line = current_line.empty() ? word : current_line + " " + word;
				float test_width = MeasureText(test_line);

				if (test_width <= _bounds.width) {
					current_line = test_line;
				}
				else {
					// Line is too long
					if (!current_line.empty()) {
						// Try to hyphenate the word if enabled
						if (_config.use_hyphenation && !current_line.empty()) {
							std::string hyphen_test = current_line + "-";
							float hyphen_width = MeasureText(hyphen_test);

							if (hyphen_width <= _bounds.width) {
								_wrapped_lines.push_back(current_line + "-");
							}
							else {
								_wrapped_lines.push_back(current_line);
							}
						}
						else {
							_wrapped_lines.push_back(current_line);
						}
						current_line = word;
					}
					else {
						// Single word is too long, force it
						current_line = word;
					}
				}
			}

			// Push remaining line
			if (!current_line.empty()) {
				_wrapped_lines.push_back(current_line);
			}
		}
	}

	void Label::WrapTextByCharacter() {
		_wrapped_lines.clear();

		if (_bounds.width <= 0.0f) {
			// No width to wrap to, just split by newlines
			std::istringstream stream(_config.text);
			std::string line;
			while (std::getline(stream, line)) {
				_wrapped_lines.push_back(line);
			}
			return;
		}

		// Split by newlines first
		std::istringstream stream(_config.text);
		std::string paragraph;

		while (std::getline(stream, paragraph)) {
			if (paragraph.empty()) {
				_wrapped_lines.push_back("");
				continue;
			}

			// Character wrap this paragraph
			std::string current_line;

			for (size_t i = 0; i < paragraph.length(); ++i) {
				char c = paragraph[i];
				std::string test_line = current_line + c;
				float test_width = MeasureText(test_line);

				if (test_width <= _bounds.width) {
					current_line += c;
				}
				else {
					// Line is too long
					if (!current_line.empty()) {
						// Add hyphen if enabled and appropriate
						if (_config.use_hyphenation && i < paragraph.length() - 1 && !std::isspace(c)) {
							std::string hyphen_test = current_line + "-";
							float hyphen_width = MeasureText(hyphen_test);

							if (hyphen_width <= _bounds.width) {
								_wrapped_lines.push_back(current_line + "-");
								current_line = c;
								continue;
							}
						}
						_wrapped_lines.push_back(current_line);
					}
					current_line = c;
				}
			}

			// Push remaining line
			if (!current_line.empty()) {
				_wrapped_lines.push_back(current_line);
			}
		}
	}

	void Label::ApplyEllipsis() {
		if (_wrapped_lines.empty())
			return;

		float line_height = static_cast<float>(_config.font_size) + _config.line_spacing;
		size_t max_lines = static_cast<size_t>(_bounds.height / line_height);

		// Handle vertical overflow
		if (_config.overflow_vertical == TextOverflow::Ellipsis && _wrapped_lines.size() > max_lines && max_lines > 0) {
			_has_overflow = true;

			// Keep only the lines that fit
			_wrapped_lines.resize(max_lines);

			// Add ellipsis to the last line
			if (!_wrapped_lines.empty()) {
				std::string& last_line = _wrapped_lines.back();
				std::string ellipsis = "...";

				// Try to fit the ellipsis
				while (!last_line.empty()) {
					std::string test_line = last_line + ellipsis;
					float test_width = MeasureText(test_line);

					if (test_width <= _bounds.width) {
						last_line += ellipsis;
						break;
					}

					// Remove last character and try again
					last_line.pop_back();
				}

				if (last_line.empty()) {
					last_line = ellipsis;
				}
			}
		}
		else if (_config.overflow_vertical == TextOverflow::Clip && _wrapped_lines.size() > max_lines && max_lines > 0) {
			_has_overflow = true;
			_wrapped_lines.resize(max_lines);
		}

		// Handle horizontal overflow
		if (_config.overflow_horizontal == TextOverflow::Ellipsis) {
			std::string ellipsis = "...";
			float ellipsis_width = MeasureText(ellipsis);

			for (auto& line : _wrapped_lines) {
				float line_width = MeasureText(line);

				if (line_width > _bounds.width) {
					_has_overflow = true;

					// Shorten the line to fit with ellipsis
					while (!line.empty()) {
						line.pop_back();
						std::string test_line = line + ellipsis;
						float test_width = MeasureText(test_line);

						if (test_width <= _bounds.width) {
							line += ellipsis;
							break;
						}
					}

					if (line.empty() || line == ellipsis) {
						line = ellipsis;
					}
				}
			}
		}
		else if (_config.overflow_horizontal == TextOverflow::Clip) {
			// For clipping, we don't modify the text - the render will handle it
			for (const auto& line : _wrapped_lines) {
				float line_width = MeasureText(line);
				if (line_width > _bounds.width) {
					_has_overflow = true;
					break;
				}
			}
		}
	}

	float Label::MeasureText(const std::string& text) const {
		raylib::Font& font = FontSystem::get_font(_config.font_index, _config.font_size, _config.font_style);
		raylib::Vector2 text_size = raylib::MeasureTextEx(font, text.c_str(), static_cast<float>(_config.font_size), 0.0f);
		return text_size.x;
	}

	bool Label::CanHyphenate(char c) const {
		// Simple heuristic: can hyphenate on vowels and consonants, but not on spaces or punctuation
		return std::isalnum(static_cast<unsigned char>(c)) != 0;
	}

} // namespace UI
