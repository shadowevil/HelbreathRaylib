#pragma once
#include <string>
#include <unordered_map>

namespace Music {
	enum TrackID : int {
		ABADDON,
		ARESDEN,
		CAROL,
		CAROL0,
		CAROL1,
		CAROL2,
		DRUNCNCITY,
		DUNGEON,
		ELVINE,
		MAIN_TM,
		MENU_MUSIC_1,
		MENU_MUSIC_2,
		MENU_MUSIC_3,
		MENU_MUSIC_4,
		MENU_MUSIC_5,
		MIDDLELAND
	};

	// Map from filename (without extension) to TrackID
	inline const std::unordered_map<std::string, TrackID> FilenameToTrackID = {
		{"abaddon", ABADDON},
		{"aresden", ARESDEN},
		{"Carol", CAROL},
		{"Carol0", CAROL0},
		{"Carol1", CAROL1},
		{"Carol2", CAROL2},
		{"druncncity", DRUNCNCITY},
		{"dungeon", DUNGEON},
		{"elvine", ELVINE},
		{"MainTm", MAIN_TM},
		{"menumusic1", MENU_MUSIC_1},
		{"menumusic2", MENU_MUSIC_2},
		{"menumusic3", MENU_MUSIC_3},
		{"menumusic4", MENU_MUSIC_4},
		{"menumusic5", MENU_MUSIC_5},
		{"middleland", MIDDLELAND}
	};
}
