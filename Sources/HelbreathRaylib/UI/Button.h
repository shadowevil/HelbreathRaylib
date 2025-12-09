#pragma once
#include "Control.h"
#include <string>
#include <optional>
#include "../FontSystem.h"
#include "../Sprite.h"

namespace UI {

	// Sprite-based button control
	class Button : public Control {
	public:
		struct SpriteConfig {
			int32_t sprite_id = -1;
			int32_t normal_frame = -1;
			int32_t hover_frame = -1;
			int32_t pressed_frame = -1;
			int32_t disabled_frame = -1;
		};

		struct TextConfig {
			std::string text;
			uint8_t font_index = 0;
			int font_size = 13;
			FontStyle font_style = FontStyle::Regular;
			raylib::Color normal_color = raylib::WHITE;
			raylib::Color hover_color = raylib::YELLOW;
			raylib::Color pressed_color = raylib::GRAY;
			raylib::Color disabled_color = raylib::DARKGRAY;
			HorizontalAlign h_align = HorizontalAlign::Center;
			VerticalAlign v_align = VerticalAlign::Middle;
		};

		Button(CSpriteCollection& sprites);
		virtual ~Button() = default;

		// Setup sprite rendering
		void SetSprite(int32_t sprite_id, int32_t normal_frame);
		void SetSprite(const SpriteConfig& config);
		void SetSpriteFromBounds(int32_t sprite_id, int32_t frame); // Auto-size from sprite

		// Setup text rendering
		void SetText(const std::string& text);
		void SetText(const TextConfig& config);

		// Rendering
		void Render() override;

	protected:
		CSpriteCollection& _sprites;
		std::optional<SpriteConfig> _sprite_config;
		std::optional<TextConfig> _text_config;

		// Get current frame based on state (virtual to allow ToggleButton override)
		virtual int32_t GetCurrentFrame() const;
		virtual raylib::Color GetCurrentTextColor() const;
	};

} // namespace UI
