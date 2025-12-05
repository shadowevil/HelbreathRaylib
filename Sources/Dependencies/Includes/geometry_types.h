#pragma once
#include <type_traits>
#include <cstdint>
#include "windows_wrapper.h"

namespace raylib {
	using Rectangle = ::Rectangle;
}

namespace rlx {
#ifndef __EMSCRIPTEN__
	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct Padding {
		T left{};
		T top{};
		T right{};
		T bottom{};
	};

	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	using Margin = Padding<T>;

	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct Rectangle {
		T x{};
		T y{};
		T width{};
		T height{};

		constexpr Rectangle() = default;

		constexpr Rectangle(T x, T y, T width, T height)
			: x(x), y(y), width(width), height(height)
		{ }

		constexpr operator raylib::Rectangle() const {
			return raylib::Rectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
		}

		constexpr Rectangle(const raylib::Rectangle& rect)
			: x(static_cast<T>(rect.x)),
			  y(static_cast<T>(rect.y)),
			  width(static_cast<T>(rect.width)),
			  height(static_cast<T>(rect.height))
		{ }

		constexpr Rectangle& operator=(const raylib::Rectangle& rect) {
			x = static_cast<T>(rect.x);
			y = static_cast<T>(rect.y);
			width = static_cast<T>(rect.width);
			height = static_cast<T>(rect.height);
			return *this;
		}

		template<typename U>
			requires std::is_integral_v<U> || std::is_floating_point_v<U>
		constexpr operator Rectangle<U>() const {
			return Rectangle<U>{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(width), static_cast<U>(height) };
		}

	#ifdef _WIN32
		constexpr operator windows::RECT() const {
			return windows::RECT{ static_cast<windows::LONG>(x), static_cast<windows::LONG>(y), static_cast<windows::LONG>(x + width), static_cast<windows::LONG>(y + height) };
		}

		constexpr Rectangle(const windows::RECT& rect)
			: x(static_cast<T>(rect.left)),
			  y(static_cast<T>(rect.top)),
			  width(static_cast<T>(rect.right - rect.left)),
			  height(static_cast<T>(rect.bottom - rect.top))
		{ }

		constexpr Rectangle& operator=(const windows::RECT& rect) {
			x = static_cast<T>(rect.left);
			y = static_cast<T>(rect.top);
			width = static_cast<T>(rect.right - rect.left);
			height = static_cast<T>(rect.bottom - rect.top);
			return *this;
		}
	#endif

		constexpr Rectangle<T> operator+(const Padding<T>& pad) const {
			return Rectangle<T>{
				x + pad.left,
				y + pad.top,
				width - (pad.left + pad.right),
				height - (pad.top + pad.bottom)
			};
		}

		constexpr Rectangle<T> operator-(const Padding<T>& pad) const {
			return Rectangle<T>{
				x - pad.left,
				y - pad.top,
				width + (pad.left + pad.right),
				height + (pad.top + pad.bottom)
			};
		}

		template<typename U>
			requires std::is_integral_v<U> || std::is_floating_point_v<U>
		constexpr bool contains(U px, U py) const {
			return (px >= static_cast<U>(x)) && (px < static_cast<U>(x + width)) &&
				(py >= static_cast<U>(y)) && (py < static_cast<U>(y + height));
		}

		template<typename U>
			requires std::is_integral_v<U> || std::is_floating_point_v<U>
		constexpr bool intersects(const Rectangle<U>&other) const {
			return !(other.x >= static_cast<U>(x + width) ||
				other.x + other.width <= static_cast<U>(x) ||
				other.y >= static_cast<U>(y + height) ||
				other.y + other.height <= static_cast<U>(y));
		}

		constexpr T right() const {
			return x + width;
		}

		constexpr T bottom() const {
			return y + height;
		}
	};

	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct MarginRectangle {
		Rectangle<T> rect{};
		Margin<T> margin{};

		constexpr MarginRectangle() = default;

		constexpr MarginRectangle(T x, T y, T width, T height, Margin<T> margin = {})
			: rect{ x, y, width, height }, margin(margin)
		{ }

		constexpr MarginRectangle(Rectangle<T> r, Margin<T> m = {})
			: rect(r), margin(m)
		{ }

		constexpr Rectangle<T> with_margin() const {
			return rect - margin; // expands outward
		}

		constexpr Rectangle<T> without_margin() const {
			return rect; // base rectangle
		}
	};

	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct PaddedRectangle {
		Rectangle<T> rect{};
		Padding<T> padding{};

		constexpr PaddedRectangle() = default;

		constexpr PaddedRectangle(T x, T y, T width, T height, Padding<T> padding = {})
			: rect{ x, y, width, height }, padding(padding)
		{ }

		constexpr PaddedRectangle(Rectangle<T> r, Padding<T> p = {})
			: rect(r), padding(p)
		{ }

		constexpr Rectangle<T> with_padding() const {
			return rect + padding; // expands inward visually (shrinks usable content)
		}

		constexpr Rectangle<T> without_padding() const {
			return rect; // base rectangle, no padding
		}
	};

	template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
	struct Box {
		Rectangle<T> rect{};
		Padding<T> padding{};
		Margin<T> margin{};

		constexpr Box() = default;

		constexpr Box(T x, T y, T width, T height, Padding<T> padding = {}, Margin<T> margin = {})
			: rect{ x, y, width, height }, padding(padding), margin(margin)
		{ }

		constexpr Box(Rectangle<T> r, Padding<T> p = {}, Margin<T> m = {})
			: rect(r), padding(p), margin(m)
		{ }

		constexpr Rectangle<T> with_padding() const {
			return rect + padding; // expands inward visually (shrinks usable content)
		}

		constexpr Rectangle<T> without_padding() const {
			return rect; // base rectangle, no padding
		}

		constexpr Rectangle<T> with_margin() const {
			return rect - margin; // expands outward
		}

		constexpr Rectangle<T> without_margin() const {
			return rect; // base rectangle
		}
	};
#else
	template<typename T>
		requires std::is_arithmetic_v<T>
	struct Padding {
		T left{};
		T top{};
		T right{};
		T bottom{};
	};

	template<typename T>
		requires std::is_arithmetic_v<T>
	using Margin = Padding<T>;

	template<typename T>
		requires std::is_arithmetic_v<T>
	struct Rectangle {
		T x{};
		T y{};
		T width{};
		T height{};

		constexpr Rectangle() = default;

		constexpr Rectangle(T x, T y, T width, T height)
			: x(x), y(y), width(width), height(height)
		{ }

		constexpr operator raylib::Rectangle() const {
			return raylib::Rectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
		}

		constexpr Rectangle(const raylib::Rectangle& rect)
			: x(static_cast<T>(rect.x)),
			  y(static_cast<T>(rect.y)),
			  width(static_cast<T>(rect.width)),
			  height(static_cast<T>(rect.height))
		{ }

		constexpr Rectangle& operator=(const raylib::Rectangle& rect) {
			x = static_cast<T>(rect.x);
			y = static_cast<T>(rect.y);
			width = static_cast<T>(rect.width);
			height = static_cast<T>(rect.height);
			return *this;
		}

		template<typename U>
			requires std::is_arithmetic_v<T>
		constexpr operator Rectangle<U>() const {
			return Rectangle<U>{ static_cast<U>(x), static_cast<U>(y), static_cast<U>(width), static_cast<U>(height) };
		}

	#ifdef _WIN32
		constexpr operator windows::RECT() const {
			return windows::RECT{ static_cast<windows::LONG>(x), static_cast<windows::LONG>(y), static_cast<windows::LONG>(x + width), static_cast<windows::LONG>(y + height) };
		}

		constexpr Rectangle(const windows::RECT& rect)
			: x(static_cast<T>(rect.left)),
			  y(static_cast<T>(rect.top)),
			  width(static_cast<T>(rect.right - rect.left)),
			  height(static_cast<T>(rect.bottom - rect.top))
		{ }

		constexpr Rectangle& operator=(const windows::RECT& rect) {
			x = static_cast<T>(rect.left);
			y = static_cast<T>(rect.top);
			width = static_cast<T>(rect.right - rect.left);
			height = static_cast<T>(rect.bottom - rect.top);
			return *this;
		}
	#endif

		constexpr Rectangle<T> operator+(const Padding<T>& pad) const {
			return Rectangle<T>{
				x + pad.left,
				y + pad.top,
				width - (pad.left + pad.right),
				height - (pad.top + pad.bottom)
			};
		}

		constexpr Rectangle<T> operator-(const Padding<T>& pad) const {
			return Rectangle<T>{
				x - pad.left,
				y - pad.top,
				width + (pad.left + pad.right),
				height + (pad.top + pad.bottom)
			};
		}

		template<typename U>
			requires std::is_arithmetic_v<T>
		constexpr bool contains(U px, U py) const {
			return (px >= static_cast<U>(x)) && (px < static_cast<U>(x + width)) &&
				(py >= static_cast<U>(y)) && (py < static_cast<U>(y + height));
		}

		template<typename U>
			requires std::is_arithmetic_v<T>
		constexpr bool intersects(const Rectangle<U>&other) const {
			return !(other.x >= static_cast<U>(x + width) ||
				other.x + other.width <= static_cast<U>(x) ||
				other.y >= static_cast<U>(y + height) ||
				other.y + other.height <= static_cast<U>(y));
		}

		constexpr T right() const {
			return x + width;
		}

		constexpr T bottom() const {
			return y + height;
		}
	};

	template<typename T>
		requires std::is_arithmetic_v<T>
	struct MarginRectangle {
		Rectangle<T> rect{};
		Margin<T> margin{};

		constexpr MarginRectangle() = default;

		constexpr MarginRectangle(T x, T y, T width, T height, Margin<T> margin = {})
			: rect{ x, y, width, height }, margin(margin)
		{ }

		constexpr MarginRectangle(Rectangle<T> r, Margin<T> m = {})
			: rect(r), margin(m)
		{ }

		constexpr Rectangle<T> with_margin() const {
			return rect - margin; // expands outward
		}

		constexpr Rectangle<T> without_margin() const {
			return rect; // base rectangle
		}
	};

	template<typename T>
		requires std::is_arithmetic_v<T>
	struct PaddedRectangle {
		Rectangle<T> rect{};
		Padding<T> padding{};

		constexpr PaddedRectangle() = default;

		constexpr PaddedRectangle(T x, T y, T width, T height, Padding<T> padding = {})
			: rect{ x, y, width, height }, padding(padding)
		{ }

		constexpr PaddedRectangle(Rectangle<T> r, Padding<T> p = {})
			: rect(r), padding(p)
		{ }

		constexpr Rectangle<T> with_padding() const {
			return rect + padding; // expands inward visually (shrinks usable content)
		}

		constexpr Rectangle<T> without_padding() const {
			return rect; // base rectangle, no padding
		}
	};

	template<typename T>
		requires std::is_arithmetic_v<T>
	struct Box {
		Rectangle<T> rect{};
		Padding<T> padding{};
		Margin<T> margin{};

		constexpr Box() = default;

		constexpr Box(T x, T y, T width, T height, Padding<T> padding = {}, Margin<T> margin = {})
			: rect{ x, y, width, height }, padding(padding), margin(margin)
		{ }

		constexpr Box(Rectangle<T> r, Padding<T> p = {}, Margin<T> m = {})
			: rect(r), padding(p), margin(m)
		{ }

		constexpr Rectangle<T> with_padding() const {
			return rect + padding; // expands inward visually (shrinks usable content)
		}

		constexpr Rectangle<T> without_padding() const {
			return rect; // base rectangle, no padding
		}

		constexpr Rectangle<T> with_margin() const {
			return rect - margin; // expands outward
		}

		constexpr Rectangle<T> without_margin() const {
			return rect; // base rectangle
		}
	};
#endif
}
