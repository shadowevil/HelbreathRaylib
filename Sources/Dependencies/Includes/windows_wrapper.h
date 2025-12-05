#pragma once

// Windows API wrapper - only include and expose what we need
#ifdef _WIN32

// Include Windows headers with minimal definitions
#ifndef _WINDOWS_WRAPPER_INCLUDE_
#	define _WINDOWS_WRAPPER_INCLUDE_
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	define NOGDI
#	define CloseWindow	CloseWindowWin32
#	define ShowCursor	ShowCursorWin32
#	define LoadImageA	LoadImageAWin32
#	include <Windows.h>
#	undef RGB
#	undef LoadImage
#	undef CloseWindow
#	undef ShowCursor
#	undef DrawText
#	undef DrawTextEx
#endif

// Windows API wrapper namespace - exposes only the functions and types we use
namespace windows {
	// Type aliases
	using HWND = ::HWND;
	using RECT = ::RECT;
	using POINT = ::POINT;
	using LONG = ::LONG;
	using DWORD = ::DWORD;
	using COLORREF = ::COLORREF;
	using BOOL = ::BOOL;
	using HMODULE = ::HMODULE;
	using LPSTR = ::LPSTR;
	using LPRECT = ::LPRECT;
	using LPPOINT = ::LPPOINT;

	// Window Functions
	inline HWND GetActiveWindow() { return ::GetActiveWindow(); }
	inline BOOL GetWindowRect(HWND hWnd, LPRECT lpRect) { return ::GetWindowRect(hWnd, lpRect); }
	inline BOOL GetClientRect(HWND hWnd, LPRECT lpRect) { return ::GetClientRect(hWnd, lpRect); }
	inline BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint) { return ::ClientToScreen(hWnd, lpPoint); }

	// Cursor Functions
	inline BOOL ClipCursor(const RECT* lpRect) { return ::ClipCursor(lpRect); }

	// Module Functions
	inline DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
		return ::GetModuleFileNameA(hModule, lpFilename, nSize);
	}
}

#endif // _WIN32
