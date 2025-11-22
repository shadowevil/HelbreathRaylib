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

namespace UnderwearColor {
	inline constexpr rlx::RGBA White       = rlx::RGBA(255, 255, 255, 255);
	inline constexpr rlx::RGBA Black       = rlx::RGBA(0, 0, 0, 255);
	inline constexpr rlx::RGBA Red         = rlx::RGBA(255, 0, 0, 255);
	inline constexpr rlx::RGBA Blue        = rlx::RGBA(0, 0, 255, 255);
	inline constexpr rlx::RGBA Green       = rlx::RGBA(0, 255, 0, 255);
	inline constexpr rlx::RGBA Yellow      = rlx::RGBA(255, 255, 0, 255);
	inline constexpr rlx::RGBA Purple		= rlx::RGBA(128, 0, 128, 255);
}
