#pragma once
#include <cstdint>
#include "windows_wrapper.h"

// Note: This file assumes raylib.h has already been included via raylib_include.h
// The Color type is properly defined in the raylib namespace via raylib_include.h

namespace rlx {
#ifndef __EMSCRIPTEN__
	struct RGB {
		constexpr RGB(uint8_t r, uint8_t g, uint8_t b) {
			rgb8 = (uint8_t)(((r & 0xE0)) | ((g >> 3) & 0x1C) | (b >> 6));
			rgb16 = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
			rgb32 = (uint32_t)(((uint8_t)(r) | ((uint16_t)((uint8_t)(g)) << 8)) | (((uint32_t)(uint8_t)(b)) << 16));
		}

		constexpr operator uint8_t() const { return rgb8; }
		constexpr operator uint16_t() const { return rgb16; }
		constexpr operator uint32_t() const { return rgb32; }
		constexpr operator raylib::Color() const {
			return { (unsigned char)(rgb32 & 0xFF), (unsigned char)((rgb32 >> 8) & 0xFF), (unsigned char)((rgb32 >> 16) & 0xFF), 255 };
		}
#ifdef _WIN32
		constexpr operator windows::COLORREF() const {
			return (windows::COLORREF)rgb32;
		}
#endif

	private:
		uint8_t rgb8;
		uint16_t rgb16;
		uint32_t rgb32;
	};

	struct RGBA {
		constexpr RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
			rgba8 = (uint8_t)(((r & 0xE0)) | ((g >> 3) & 0x1C) | (b >> 6));
			rgba16 = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
			rgba32 = (uint32_t)(r | (g << 8) | (b << 16) | (a << 24));
		}

		constexpr uint8_t GetR() const { return  rgba32 & 0xFF; }
		constexpr uint8_t GetG() const { return (rgba32 >> 8) & 0xFF; }
		constexpr uint8_t GetB() const { return (rgba32 >> 16) & 0xFF; }
		constexpr uint8_t GetA() const { return (rgba32 >> 24) & 0xFF; }

		constexpr operator uint8_t()  const { return rgba8; }
		constexpr operator uint16_t() const { return rgba16; }
		constexpr operator uint32_t() const { return rgba32; }

		constexpr operator raylib::Color() const {
			return {
				(unsigned char)GetR(),
				(unsigned char)GetG(),
				(unsigned char)GetB(),
				(unsigned char)GetA()
			};
		}

#ifdef _WIN32
		constexpr operator windows::COLORREF() const {
			return (windows::COLORREF)(rgba32 & 0x00FFFFFF);
		}
#endif

		static constexpr RGBA Lerp(const RGBA& a, const RGBA& b, float t)
		{
			uint8_t r = (uint8_t)(a.GetR() + (b.GetR() - a.GetR()) * t);
			uint8_t g = (uint8_t)(a.GetG() + (b.GetG() - a.GetG()) * t);
			uint8_t bl = (uint8_t)(a.GetB() + (b.GetB() - a.GetB()) * t);
			uint8_t al = (uint8_t)(a.GetA() + (b.GetA() - a.GetA()) * t);

			return RGBA(r, g, bl, al);
		}

		static constexpr rlx::RGBA Lerp3(const rlx::RGBA& A,
			const rlx::RGBA& B, const rlx::RGBA& C, float t)
		{
			float mid = 0.5f;

			// First half: A -> B
			rlx::RGBA AB = rlx::RGBA::Lerp(A, B, t / mid);

			// Second half: B -> C
			rlx::RGBA BC = rlx::RGBA::Lerp(B, C, (t - mid) / (1.0f - mid));

			// Blend the two halves together
			return (t <= mid) ? AB : BC;
		}

	private:
		uint8_t  rgba8;
		uint16_t rgba16;
		uint32_t rgba32;
	};
#else
	struct RGB {
		constexpr RGB(uint8_t r, uint8_t g, uint8_t b)
			:
			rgb8((uint8_t)(((r & 0xE0)) | ((g >> 3) & 0x1C) | (b >> 6))),
			rgb16((uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))),
			rgb32((uint32_t)(((uint8_t)(r) | ((uint16_t)((uint8_t)(g)) << 8)) | (((uint32_t)(uint8_t)(b)) << 16)))
		{ }

		constexpr operator uint8_t() const { return rgb8; }
		constexpr operator uint16_t() const { return rgb16; }
		constexpr operator uint32_t() const { return rgb32; }
		constexpr operator raylib::Color() const {
			return { (unsigned char)(rgb32 & 0xFF), (unsigned char)((rgb32 >> 8) & 0xFF), (unsigned char)((rgb32 >> 16) & 0xFF), 255 };
		}
#ifdef _WIN32
		constexpr operator windows::COLORREF() const {
			return (windows::COLORREF)rgb32;
		}
#endif

	private:
		uint8_t rgb8;
		uint16_t rgb16;
		uint32_t rgb32;
	};

	struct RGBA {
		constexpr RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			:
			rgba8((uint8_t)(((r & 0xE0)) | ((g >> 3) & 0x1C) | (b >> 6))),
			rgba16((uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))),
			rgba32((uint32_t)(r | (g << 8) | (b << 16) | (a << 24)))
		{ }

		constexpr uint8_t GetR() const { return  rgba32 & 0xFF; }
		constexpr uint8_t GetG() const { return (rgba32 >> 8) & 0xFF; }
		constexpr uint8_t GetB() const { return (rgba32 >> 16) & 0xFF; }
		constexpr uint8_t GetA() const { return (rgba32 >> 24) & 0xFF; }

		constexpr operator uint8_t()  const { return rgba8; }
		constexpr operator uint16_t() const { return rgba16; }
		constexpr operator uint32_t() const { return rgba32; }

		constexpr operator raylib::Color() const {
			return {
				(unsigned char)GetR(),
				(unsigned char)GetG(),
				(unsigned char)GetB(),
				(unsigned char)GetA()
			};
		}

#ifdef _WIN32
		constexpr operator windows::COLORREF() const {
			return (windows::COLORREF)(rgba32 & 0x00FFFFFF);
		}
#endif

		static constexpr RGBA Lerp(const RGBA& a, const RGBA& b, float t)
		{
			uint8_t r = (uint8_t)(a.GetR() + (b.GetR() - a.GetR()) * t);
			uint8_t g = (uint8_t)(a.GetG() + (b.GetG() - a.GetG()) * t);
			uint8_t bl = (uint8_t)(a.GetB() + (b.GetB() - a.GetB()) * t);
			uint8_t al = (uint8_t)(a.GetA() + (b.GetA() - a.GetA()) * t);

			return RGBA(r, g, bl, al);
		}

		static constexpr rlx::RGBA Lerp3(const rlx::RGBA& A,
			const rlx::RGBA& B, const rlx::RGBA& C, float t)
		{
			float mid = 0.5f;

			// First half: A -> B
			rlx::RGBA AB = rlx::RGBA::Lerp(A, B, t / mid);

			// Second half: B -> C
			rlx::RGBA BC = rlx::RGBA::Lerp(B, C, (t - mid) / (1.0f - mid));

			// Blend the two halves together
			return (t <= mid) ? AB : BC;
		}

	private:
		uint8_t  rgba8;
		uint16_t rgba16;
		uint32_t rgba32;
	};
#endif
}
