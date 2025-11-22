#pragma once

enum SpriteID : size_t {
	SPRID_LOADINGSCREEN				= 0,
	SPRID_MAINMENUSCREEN			= 1,
	SPRID_EXITSCREEN				= 2,
	SPRID_NEWACCOUNTSCREEN			= 3,
	SPRID_LOGINSCREEN				= 4,
	SPRID_CHARACTERSELECTSCREEN		= 5,
	SPRID_CREATECHARARCTERSCREEN	= 6,

	SPRID_MOUSECURSOR				= 7,
	SPRID_BUTTONS					= 8,
	
	SPRID_EQUIP_MODEL				= 9,
	SPRID_EQUIP_HAIRSTYLE			= 10,
	SPRID_EQUIP_UNDERWEAR			= 11,
	SPRID_ITEM_ATLAS_EQUIP			= 12,
	SPRID_ITEM_ATLAS_GROUND			= 13,
	SPRID_ITEM_ATLAS_PACK			= 14
};

enum ModelID : size_t {
	MDLID_MALE_START = 0,
	MDLID_MALE_GAME_MODEL = 0,
	MDLID_MALE_GAME_MODEL_END = 95,
	MDLID_MALE_HAIRSTYLE_START = 96,
	MDLID_MALE_HAIRSTYLE_1 = 96,
	MDLID_MALE_HAIRSTYLE_2 = 108,
	MDLID_MALE_HAIRSTYLE_3 = 120,
	MDLID_MALE_HAIRSTYLE_4 = 132,
	MDLID_MALE_HAIRSTYLE_5 = 144,
	MDLID_MALE_HAIRSTYLE_6 = 156,
	MDLID_MALE_HAIRSTYLE_7 = 168,
	MDLID_MALE_UNDERWEAR = 180,
	MDLID_MALE_ITEM_START = 200,

	MDLID_FEMALE_START = 5000,
	MDLID_FEMALE_GAME_MODEL = 5000,
	MDLID_FEMALE_GAME_MODEL_END = 5095,
	MDLID_FEMALE_HAIRSTYLE_START = 5096,
	MDLID_FEMALE_HAIRSTYLE_1 = 5096,
	MDLID_FEMALE_HAIRSTYLE_2 = 5108,
	MDLID_FEMALE_HAIRSTYLE_3 = 5120,
	MDLID_FEMALE_HAIRSTYLE_4 = 5132,
	MDLID_FEMALE_HAIRSTYLE_5 = 5144,
	MDLID_FEMALE_HAIRSTYLE_6 = 5156,
	MDLID_FEMALE_HAIRSTYLE_7 = 5168,
	MDLID_FEMALE_UNDERWEAR = 5180,
	MDLID_FEMALE_ITEM_START = 5200
};

struct SPR_LOADINGSCREEN {
	static constexpr size_t PAK_INDEX		= 0;
	static constexpr size_t BACKGROUND		= 0;
	static constexpr size_t PROGRESS_BAR	= 1;
};

struct SPR_MAINMENUSCREEN {
	static constexpr size_t PAK_INDEX		= 1;
	static constexpr size_t BACKGROUND		= 0;
	static constexpr size_t BUTTON_1		= 1;
	static constexpr size_t BUTTON_2		= 2;
	static constexpr size_t BUTTON_3		= 3;
};

struct SPR_EXITSCREEN {
	static constexpr size_t PAK_INDEX		= 2;
	static constexpr size_t BACKGROUND		= 0;
	static constexpr size_t OVERLAY			= 1;
};

struct SPR_NEWACCOUNTSCREEN {
	static constexpr size_t PAK_INDEX		= 2;
	static constexpr size_t BACKGROUND		= 0;
};

struct SPR_LOGINSCREEN {
	static constexpr size_t PAK_INDEX				= 3;
	static constexpr size_t BACKGROUND				= 0;
	static constexpr size_t OVERLAY_SELECT_SERVER	= 1;
	static constexpr size_t OVERLAY_LOGIN			= 2;
	static constexpr size_t BUTTON_CONNECT			= 3;
	static constexpr size_t BUTTON_CANCEL			= 4;
	static constexpr size_t BUTTON_ABADDON			= 5;
	static constexpr size_t BUTTON_APOCOLYPSE		= 6;
};

struct SPR_CHARACTERSELECTSCREEN {
	static constexpr size_t PAK_INDEX			= 4;
	static constexpr size_t BACKGROUND			= 0;
	static constexpr size_t SELECTED_CHARACTER	= 1;
};

struct SPR_CREATECHARARCTERSCREEN {
	static constexpr size_t PAK_INDEX			= 5;
	static constexpr size_t BACKGROUND			= 0;
	static constexpr size_t BUTTON_ARROW_LEFT	= 1;
	static constexpr size_t BUTTON_ARROW_RIGHT	= 2;
	static constexpr size_t BUTTON_PLUS			= 3;
	static constexpr size_t BUTTON_MINUS		= 4;
};

struct SPR_MOUSECURSOR {
	static constexpr size_t PAK_INDEX				= 0;
	static constexpr size_t DEFAULT					= 0;
	static constexpr size_t GRAB_ANIMATED			= 1;
	static constexpr size_t ATTACK					= 3;
	static constexpr size_t MAGIC_FRIENDLY			= 4;
	static constexpr size_t MAGIC_ENEMY				= 5;
	static constexpr size_t HAND_FRIENDLY			= 6;
	static constexpr size_t HAND_ENEMY				= 7;
	static constexpr size_t WAITING					= 8;
	static constexpr size_t ITEM_TARGETING_ANIMATED = 9;
	static constexpr size_t NONE					= 12;
};

struct SPR_BUTTONS {
	static constexpr size_t PAK_INDEX					= 1;
	static constexpr size_t SMALL_BUTTON				= 0;
	static constexpr size_t SMALL_BUTTON_HOVER			= 4;
	static constexpr size_t MEDIUM_BUTTON				= 1;
	static constexpr size_t MEIDUM_BUTTON_HOVER			= 5;
	static constexpr size_t LARGE_BUTTON				= 2;
	static constexpr size_t LARGE_BUTTON_HOVER			= 6;
	static constexpr size_t EXTRA_LARGE_BUTTON			= 3;
	static constexpr size_t EXTRA_LARGE_BUTTON_HOVER	= 7;
};

struct SPR_EQUIP {
	static constexpr size_t PAK_INDEX_MODEL	= 0;
	static constexpr size_t PAK_INDEX_HAIRSTYLE = 1;
	static constexpr size_t PAK_INDEX_UNDERWEAR = 2;
};

struct SPR_GAME_ANIMATIONS {
	static constexpr size_t PAK_INDEX_STANDING_PEACE	= 0;
	static constexpr size_t PAK_INDEX_STANDING_COMBAT	= 1;
	static constexpr size_t PAK_INDEX_WALKING_PEACE		= 2;
	static constexpr size_t PAK_INDEX_WALKING_COMBAT	= 3;
	static constexpr size_t PAK_INDEX_RUNNING			= 4;
	static constexpr size_t PAK_INDEX_ATTACKING_PEACE	= 5;
	static constexpr size_t PAK_INDEX_ATTACKING_HAND	= 6;
	static constexpr size_t PAK_INDEX_ATTACKING_BOW		= 7;
	static constexpr size_t PAK_INDEX_ATTACKING_CAST	= 8;
	static constexpr size_t PAK_INDEX_PICKUP			= 9;
	static constexpr size_t PAK_INDEX_TAKEDAMAGE		= 10;
	static constexpr size_t PAK_INDEX_DYING				= 11;
};