#pragma once
#include <string>
#include <cstdint>

namespace constant {
	inline constexpr int BASE_WIDTH = 640;
	inline constexpr int BASE_HEIGHT = 480;
	inline constexpr int UPSCALE_FACTOR = 4;

	inline const std::string SPRITE_PATH		= "SPRITES";
	inline const std::string FONT_PATH			= "FONTS";
	inline const std::string SPRITE_IMAGE_TYPE	= ".png";

	inline constexpr double SPRITE_UNLOAD_TIMEOUT_SECONDS = 60.0;
}