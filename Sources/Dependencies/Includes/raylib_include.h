#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include <typeinfo>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#define NOGDI
	#define CloseWindow	CloseWindowWin32
	#define ShowCursor	ShowCursorWin32
	#define LoadImageA	LoadImageAWin32
	#include "Windows.h"
	#undef RGB
	#undef LoadImage
	#undef CloseWindow
	#undef ShowCursor
	#undef DrawText
	#undef DrawTextEx
#endif
#define Rectangle rlRectangle
extern "C" {
#ifdef __EMSCRIPTEN__
    #include "web_include/raylib.h"
    #include "web_include/rlgl.h"
#else
    #include "raylib.h"
    #include "rlgl.h"
#endif
}
#ifdef __EMSCRIPTEN__
    #include "web_include/raymath.h"
#else
    #include "raymath.h"
#endif
#undef Rectangle
#include <filesystem>
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <fstream>
#include <sstream>

inline std::string get_executable_path() {
#if defined(_WIN32)
	char path[MAX_PATH];
	DWORD len = GetModuleFileNameA(nullptr, path, MAX_PATH);
	if (len == 0 || len == MAX_PATH)
		throw std::runtime_error("Failed to get executable path");
	return std::string(path);

#elif defined(__APPLE__)
	char path[PATH_MAX];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) != 0)
		throw std::runtime_error("Failed to get executable path");
	char resolved[PATH_MAX];
	if (realpath(path, resolved) == nullptr)
		throw std::runtime_error("Failed to resolve executable path");
	return std::string(resolved);

#elif defined(__linux__)
	char path[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (len == -1)
		throw std::runtime_error("Failed to get executable path");
	path[len] = '\0';
	return std::string(path);
#elif defined(__EMSCRIPTEN__)
	return "";
#endif
}

inline std::string get_executable_dir() {
#ifdef _DEBUG
	return "";
#endif
	std::string exe = get_executable_path();
#if defined(_WIN32)
	const char sep = '\\';
#else
	const char sep = '/';
#endif
	size_t pos = exe.find_last_of(sep);
	return (pos != std::string::npos) ? exe.substr(0, pos) : exe;
}

#if defined(_WIN32)
constexpr char PATH_SEP = '\\';
constexpr char ALT_SEP = '/';
#else
constexpr char PATH_SEP = '/';
constexpr char ALT_SEP = '\\';
#endif

inline std::string sanitize_path(std::string path) {
	for (auto& ch : path) {
		if (ch == ALT_SEP)
			ch = PATH_SEP;
	}
	return path;
}

template<typename T>
inline void append_path_part(std::string& result, const T& part) {
	std::string sanitized = sanitize_path(part);
	if (result.empty()) {
		result = sanitized;
		return;
	}
	if (result.back() != PATH_SEP)
		result += PATH_SEP;
	result += sanitized;
}

template<typename... Args>
inline std::string path_combine(const Args&... args) {
	static_assert(sizeof...(args) > 0, "path_combine requires at least one argument");

	// Runtime check fallback for C++14
	std::string result;
	int dummy[] = { 0, (append_path_part(result, std::string(args)), 0)... };
	(void)dummy;
	return result;
}

inline std::string get_filename(const std::string& path) {
	if (path.empty())
		return {};

	size_t pos1 = path.find_last_of(PATH_SEP);
	size_t pos2 = path.find_last_of(ALT_SEP);
	size_t pos = std::string::npos;

	if (pos1 != std::string::npos && pos2 != std::string::npos)
		pos = (pos1 > pos2) ? pos1 : pos2;
	else if (pos1 != std::string::npos)
		pos = pos1;
	else if (pos2 != std::string::npos)
		pos = pos2;

	return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}

inline std::string get_filename_no_ext(const std::string& path) {
	if (path.empty())
		return {};

	size_t pos1 = path.find_last_of(PATH_SEP);
	size_t pos2 = path.find_last_of(ALT_SEP);
	size_t pos = std::string::npos;

	if (pos1 != std::string::npos && pos2 != std::string::npos)
		pos = (pos1 > pos2) ? pos1 : pos2;
	else if (pos1 != std::string::npos)
		pos = pos1;
	else if (pos2 != std::string::npos)
		pos = pos2;

	std::string filename = (pos != std::string::npos) ? path.substr(pos + 1) : path;

	size_t dot = filename.find_last_of('.');
	if (dot != std::string::npos)
		filename = filename.substr(0, dot);

	return filename;
}

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
		constexpr operator Color() const {
			return { (unsigned char)(rgb32 & 0xFF), (unsigned char)((rgb32 >> 8) & 0xFF), (unsigned char)((rgb32 >> 16) & 0xFF), 255 };
		}
#ifdef _WIN32
		constexpr operator COLORREF() const {
			return (COLORREF)rgb32;
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

		constexpr operator Color() const {
			return {
				(unsigned char)GetR(),
				(unsigned char)GetG(),
				(unsigned char)GetB(),
				(unsigned char)GetA()
			};
		}

#ifdef _WIN32
		constexpr operator COLORREF() const {
			return (COLORREF)(rgba32 & 0x00FFFFFF);
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

		constexpr operator rlRectangle() const {
			return rlRectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
		}

		constexpr Rectangle(const rlRectangle& rect) 
			: x(static_cast<T>(rect.x)),
			  y(static_cast<T>(rect.y)),
			  width(static_cast<T>(rect.width)),
			  height(static_cast<T>(rect.height))
		{ }

		constexpr Rectangle& operator=(const rlRectangle& rect) {
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
		constexpr operator RECT() const {
			return RECT{ static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x + width), static_cast<LONG>(y + height) };
		}

		constexpr Rectangle(const RECT& rect)
			: x(static_cast<T>(rect.left)),
			  y(static_cast<T>(rect.top)),
			  width(static_cast<T>(rect.right - rect.left)),
			  height(static_cast<T>(rect.bottom - rect.top))
		{ }

		constexpr Rectangle& operator=(const RECT& rect) {
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
		constexpr operator Color() const {
			return { (unsigned char)(rgb32 & 0xFF), (unsigned char)((rgb32 >> 8) & 0xFF), (unsigned char)((rgb32 >> 16) & 0xFF), 255 };
		}
#ifdef _WIN32
		constexpr operator COLORREF() const {
			return (COLORREF)rgb32;
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

		constexpr operator Color() const {
			return {
				(unsigned char)GetR(),
				(unsigned char)GetG(),
				(unsigned char)GetB(),
				(unsigned char)GetA()
			};
		}

#ifdef _WIN32
		constexpr operator COLORREF() const {
			return (COLORREF)(rgba32 & 0x00FFFFFF);
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

		constexpr operator rlRectangle() const {
			return rlRectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
		}

		constexpr Rectangle(const rlRectangle& rect) 
			: x(static_cast<T>(rect.x)),
			  y(static_cast<T>(rect.y)),
			  width(static_cast<T>(rect.width)),
			  height(static_cast<T>(rect.height))
		{ }

		constexpr Rectangle& operator=(const rlRectangle& rect) {
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
		constexpr operator RECT() const {
			return RECT{ static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x + width), static_cast<LONG>(y + height) };
		}

		constexpr Rectangle(const RECT& rect)
			: x(static_cast<T>(rect.left)),
			  y(static_cast<T>(rect.top)),
			  width(static_cast<T>(rect.right - rect.left)),
			  height(static_cast<T>(rect.bottom - rect.top))
		{ }

		constexpr Rectangle& operator=(const RECT& rect) {
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

	namespace File
	{
		inline bool Exists(const std::filesystem::path& filePath) {
			return std::filesystem::exists(filePath);
		}

		inline bool Delete(const std::filesystem::path& filePath) {
			return std::filesystem::remove(filePath);
		}

		inline bool Copy(const std::filesystem::path& sourcePath, const std::filesystem::path& destPath, bool overwrite = false) {
			std::filesystem::copy_options options = std::filesystem::copy_options::none;
			if (overwrite) {
				options |= std::filesystem::copy_options::overwrite_existing;
			}

			return std::filesystem::copy_file(sourcePath, destPath, options);
		}

		inline void Move(const std::filesystem::path& sourcePath, const std::filesystem::path& destPath, bool overwrite = false) {
			if (overwrite && std::filesystem::exists(destPath)) {
				std::filesystem::remove(destPath);
			}

			std::filesystem::rename(sourcePath, destPath);
		}

		inline std::string ReadAllText(const std::filesystem::path& filePath) {
			std::ifstream file(filePath, std::ios::in | std::ios::binary);
			if (!file) {
				throw std::runtime_error("Failed to open file for reading: " + filePath.string());
			}
			std::ostringstream contents;
			contents << file.rdbuf();
			return contents.str();
		}
	}

	namespace Directory
	{
		inline bool Exists(const std::filesystem::path& dirPath) {
			return std::filesystem::exists(dirPath);
		}

		inline bool Create(const std::filesystem::path& dirPath) {
			return std::filesystem::create_directory(dirPath);
		}

		inline bool Delete(const std::filesystem::path& dirPath) {
			return std::filesystem::remove(dirPath);
		}

		inline std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& dirPath) {
			std::vector<std::filesystem::path> files;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_regular_file()) {
					files.push_back(entry.path());
				}
			}
			return files;
		}

		inline std::vector<std::filesystem::path> GetDirectories(const std::filesystem::path& dirPath) {
			std::vector<std::filesystem::path> directories;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_directory()) {
					directories.push_back(entry.path());
				}
			}
			return directories;
		}

		inline std::vector<std::filesystem::path> GetFilesWithExtension(const std::filesystem::path& dirPath, const std::string& extension) {
			std::vector<std::filesystem::path> files;
			for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
				if (entry.is_regular_file() && entry.path().extension() == extension) {
					files.push_back(entry.path());
				}
			}
			return files;
		}
	}

	inline void BeginUpscaleRender(RenderTexture2D target, float scale = 1.0f)
	{
		BeginTextureMode(target);
		BeginMode2D(Camera2D{ .zoom = scale });
	}

	inline float GetScale(uint16_t base_x, uint16_t base_y)
	{
		float windowW = (float)GetScreenWidth();
		float windowH = (float)GetScreenHeight();
		float scale = std::min(windowW / base_x, windowH / base_y);
		return scale;
	}

	inline std::tuple<float, float, float, float> GetUpscaledTargetArea(int width, int height)
	{
		float scale = GetScale(width, height);
		float renderW = width * scale;
		float renderH = height * scale;
		float offsetX = (GetScreenWidth() - renderW) * 0.5f;
		float offsetY = (GetScreenHeight() - renderH) * 0.5f;
		return { offsetX, offsetY, renderW, renderH };
	}

	inline void EndUpscaleRender(RenderTexture2D target, Color background = BLACK, std::function<void()> before = nullptr, std::function<void()> after = nullptr)
	{
		EndMode2D();
		EndTextureMode();
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(target.texture.width, target.texture.height);
		BeginDrawing();
		ClearBackground(background);
		if (before)
			before();
		DrawTexturePro(
			target.texture,
			{ 0, 0, (float)target.texture.width, -(float)target.texture.height },
			{ offsetX, offsetY, renderW, renderH },
			{ 0, 0 },
			0,
			WHITE
		);
		if (after)
			after();
		EndDrawing();
	}

	template<typename T>
	inline bool RectangleContainsScaledMouse(const rlx::Rectangle<T>& rect, uint16_t base_x, uint16_t base_y)
	{
		float scale = GetScale(base_x, base_y);
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(base_x, base_y);

		rlx::Rectangle<T> scaled_rect{
			static_cast<T>(rect.x * scale + offsetX),
			static_cast<T>(rect.y * scale + offsetY),
			static_cast<T>(rect.width * scale),
			static_cast<T>(rect.height * scale)
		};

		auto [mouseX, mouseY] = GetMousePosition();
		return scaled_rect.contains(mouseX, mouseY);
	}

	inline Vector2 GetScaledMousePosition(uint16_t base_x, uint16_t base_y)
	{
		float scale = GetScale(base_x, base_y);
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(base_x, base_y);
		auto [mouseX, mouseY] = GetMousePosition();
		float scaledX = (mouseX - offsetX) / scale;
		float scaledY = (mouseY - offsetY) / scale;

		// Clamp to valid game area (0 to base_x/base_y)
		scaledX = std::max(0.0f, std::min(scaledX, (float)base_x));
		scaledY = std::max(0.0f, std::min(scaledY, (float)base_y));

		return Vector2{ scaledX, scaledY };
	}

	inline void LockCursor(int width, int height) {
#ifdef _WIN32
		auto [offsetX, offsetY, renderW, renderH] = GetUpscaledTargetArea(width, height);

		HWND hwnd = GetActiveWindow();
		RECT windowRect, clientRect;
		GetWindowRect(hwnd, &windowRect);
		GetClientRect(hwnd, &clientRect);

		POINT clientTopLeft = { 0, 0 };
		ClientToScreen(hwnd, &clientTopLeft);

		LONG borderX = clientTopLeft.x - windowRect.left;
		LONG borderY = clientTopLeft.y - windowRect.top;

		RECT clipRect;
		clipRect.left = windowRect.left + borderX + static_cast<LONG>(offsetX);
		clipRect.top = windowRect.top + borderY + static_cast<LONG>(offsetY);
		clipRect.right = clipRect.left + static_cast<LONG>(renderW);
		clipRect.bottom = clipRect.top + static_cast<LONG>(renderH);

		ClipCursor(&clipRect);
#endif
		// Note: Pointer Lock API disabled for Emscripten
		// Point-and-click games need normal mouse position tracking
	}

	inline void UnlockCursor() {
#ifdef _WIN32
		ClipCursor(nullptr);
#endif
		// Note: Pointer Lock API disabled for Emscripten
	}

	inline bool HasElapsed(double timer, double interval)
	{
		return GetTime() - timer >= interval;
	}
}

namespace stdx {
	inline std::string to_lower(const std::string& str) {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return result;
	}
}