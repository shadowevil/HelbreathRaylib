#pragma once
#include <cstdint>
#include "raylib_include.h"

namespace GameColor {

}

namespace SkinColor {
	inline constexpr rlx::RGBA Base_White		= rlx::RGBA(255, 255, 255, 255);
	inline constexpr rlx::RGBA Base_Black		= rlx::RGBA(90, 65, 40, 255);
	inline constexpr rlx::RGBA Base_Yellow	    = rlx::RGBA(255, 245, 200, 255);

	// White => Black
	inline constexpr rlx::RGBA WhiteTone0            = rlx::RGBA::Lerp(Base_White, Base_Black, 0.00f);
	inline constexpr rlx::RGBA WhiteTone1            = rlx::RGBA::Lerp(Base_White, Base_Black, 0.25f);
	inline constexpr rlx::RGBA WhiteTone2            = rlx::RGBA::Lerp(Base_White, Base_Black, 0.50f);
	inline constexpr rlx::RGBA WhiteTone3            = rlx::RGBA::Lerp(Base_White, Base_Black, 0.75f);
	inline constexpr rlx::RGBA WhiteTone4            = rlx::RGBA::Lerp(Base_White, Base_Black, 1.00f);

	// Yellow => Black
	inline constexpr rlx::RGBA YellowTone0            = rlx::RGBA::Lerp(Base_Yellow, Base_Black, 0.00f);
	inline constexpr rlx::RGBA YellowTone1            = rlx::RGBA::Lerp(Base_Yellow, Base_Black, 0.25f);
	inline constexpr rlx::RGBA YellowTone2            = rlx::RGBA::Lerp(Base_Yellow, Base_Black, 0.50f);
	inline constexpr rlx::RGBA YellowTone3            = rlx::RGBA::Lerp(Base_Yellow, Base_Black, 0.75f);
	inline constexpr rlx::RGBA YellowTone4            = rlx::RGBA::Lerp(Base_Yellow, Base_Black, 1.00f);

	// White => Yellow
	inline constexpr rlx::RGBA YhiteTone0           = rlx::RGBA::Lerp(Base_White, Base_Yellow, 0.00f);
	inline constexpr rlx::RGBA YhiteTone1           = rlx::RGBA::Lerp(Base_White, Base_Yellow, 0.25f);
	inline constexpr rlx::RGBA YhiteTone2           = rlx::RGBA::Lerp(Base_White, Base_Yellow, 0.50f);
	inline constexpr rlx::RGBA YhiteTone3           = rlx::RGBA::Lerp(Base_White, Base_Yellow, 0.75f);
	inline constexpr rlx::RGBA YhiteTone4           = rlx::RGBA::Lerp(Base_White, Base_Yellow, 1.00f);
}
using SkinColorIndex = uint8_t;
inline constexpr rlx::RGBA SkinColorFromIndex(SkinColorIndex index) {
	switch (index) {
	case 0: return SkinColor::WhiteTone0;
	case 1: return SkinColor::WhiteTone1;
	case 2: return SkinColor::WhiteTone2;
	case 3: return SkinColor::WhiteTone3;
	case 4: return SkinColor::WhiteTone4;
	case 5: return SkinColor::YellowTone0;
	case 6: return SkinColor::YellowTone1;
	case 7: return SkinColor::YellowTone2;
	case 8: return SkinColor::YellowTone3;
	case 9: return SkinColor::YellowTone4;
	default: return SkinColor::WhiteTone2;
	}
}
inline constexpr size_t SkinColorCount = 10;

namespace HairColor {
    inline constexpr rlx::RGBA Black       = rlx::RGBA(10, 10, 10, 255);
    inline constexpr rlx::RGBA DarkBrown   = rlx::RGBA(70, 40, 20, 255);

    inline constexpr rlx::RGBA Brown       = rlx::RGBA(97, 56, 26, 255);      // 130,75,35 * 0.75
    inline constexpr rlx::RGBA LightBrown  = rlx::RGBA(120, 82, 41, 255);     // 160,110,55 * 0.75
    inline constexpr rlx::RGBA Blonde      = rlx::RGBA(150, 127, 52, 255);    // 200,170,70 * 0.75
    inline constexpr rlx::RGBA Red         = rlx::RGBA(112, 26, 11, 255);     // 150,35,15 * 0.75
    inline constexpr rlx::RGBA Gray        = rlx::RGBA(90, 90, 90, 255);       // 120,120,120 * 0.75
    inline constexpr rlx::RGBA White       = rlx::RGBA(172, 172, 172, 255);    // 230,230,230 * 0.75
}
using HairColorIndex = uint8_t;
inline constexpr rlx::RGBA HairColorFromIndex(HairColorIndex index) {
	switch (index) {
	case 0: return HairColor::Black;
	case 1: return HairColor::DarkBrown;
	case 2: return HairColor::Brown;
	case 3: return HairColor::LightBrown;
	case 4: return HairColor::Blonde;
	case 5: return HairColor::Red;
	case 6: return HairColor::Gray;
	case 7: return HairColor::White;
	default: return HairColor::Black;
	}
}
inline constexpr size_t HairColorCount = 8;

namespace UnderwearColor {
	inline constexpr rlx::RGBA White       = rlx::RGBA(255, 255, 255, 255);
	inline constexpr rlx::RGBA Black       = rlx::RGBA(0, 0, 0, 255);
	inline constexpr rlx::RGBA Red         = rlx::RGBA(255, 0, 0, 255);
	inline constexpr rlx::RGBA Blue        = rlx::RGBA(0, 0, 255, 255);
	inline constexpr rlx::RGBA Green       = rlx::RGBA(0, 255, 0, 255);
	inline constexpr rlx::RGBA Yellow      = rlx::RGBA(255, 255, 0, 255);
	inline constexpr rlx::RGBA Purple		= rlx::RGBA(128, 0, 128, 255);
}
using UnderwearColorIndex = uint8_t;
inline constexpr rlx::RGBA UnderwearColorFromIndex(UnderwearColorIndex index) {
	switch (index) {
	case 0: return UnderwearColor::White;
	case 1: return UnderwearColor::Black;
	case 2: return UnderwearColor::Red;
	case 3: return UnderwearColor::Blue;
	case 4: return UnderwearColor::Green;
	case 5: return UnderwearColor::Yellow;
	case 6: return UnderwearColor::Purple;
	default: return UnderwearColor::White;
	}
}
inline constexpr size_t UnderwearColorCount = 7;
