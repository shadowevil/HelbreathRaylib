#pragma once

enum SpriteID : size_t {
	SPRID_LOADINGSCREEN				= 0,
	SPRID_MAINMENUSCREEN			= 1,
	SPRID_EXITSCREEN				= 2,
	SPRID_NEWACCOUNTSCREEN			= 3,
	SPRID_LOGINSCREEN				= 4,
	SPRID_CHARACTERSELECTSCREEN		= 5,
	SPRID_CREATECHARARCTERSCREEN	= 6
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