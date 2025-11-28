#pragma once
#include <string>
#include <cstdint>
#include <filesystem>

namespace constant {
	inline constexpr int BASE_WIDTH = 640;
	inline constexpr int BASE_HEIGHT = 480;
	inline constexpr int UPSCALE_FACTOR = 8;

	inline const std::filesystem::path SPRITE_PATH					= "SPRITES";
	inline const std::filesystem::path SPRITE_MALE_PATH				= SPRITE_PATH / "Male";
	inline const std::filesystem::path SPRITE_FEMALE_PATH			= SPRITE_PATH / "Female";
	inline const std::filesystem::path SPRITE_MAP_PATH				= SPRITE_PATH / "Maps";
	inline const std::filesystem::path SPRITE_ITEM_PATH				= SPRITE_PATH / "Items";
	inline const std::filesystem::path FONT_PATH					= "FONTS";
	inline const std::filesystem::path MAPDATA_PATH					= "MAPDATA";
	inline const std::filesystem::path MUSIC_PATH					= "MUSIC";
	inline const std::filesystem::path SOUND_PATH					= "SOUNDS";
	inline const std::filesystem::path DATA_PATH					= "DATA";
	inline const std::filesystem::path SAVE_GAME_PATH				= "SAVEDGAMES";

	inline const std::string SPRITE_IMAGE_TYPE				= ".png";
	inline constexpr double SPRITE_UNLOAD_TIMEOUT_SECONDS	= 60.0;

	inline constexpr uint32_t TILE_SIZE						= 32;
	inline constexpr uint32_t TILE_HALF						= TILE_SIZE / 2;
}