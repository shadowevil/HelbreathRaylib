#pragma once
#include <string>
#include <stdexcept>
#include "windows_wrapper.h"

#if defined(__APPLE__)
	#include <mach-o/dyld.h>
	#include <limits.h>
#elif defined(__linux__)
	#include <unistd.h>
	#include <limits.h>
#endif

// Cross-platform maximum path length
#if defined(_WIN32)
	constexpr int PLATFORM_MAX_PATH = MAX_PATH;  // 260 on Windows
#elif defined(__APPLE__) || defined(__linux__)
	constexpr int PLATFORM_MAX_PATH = PATH_MAX;  // 4096 on most Unix systems
#elif defined(__EMSCRIPTEN__)
	constexpr int PLATFORM_MAX_PATH = 4096;      // Typical value for web/Unix-like
#else
	constexpr int PLATFORM_MAX_PATH = 260;       // Conservative fallback
#endif

inline std::string get_executable_path() {
#if defined(_WIN32)
	char path[PLATFORM_MAX_PATH];
	windows::DWORD len = windows::GetModuleFileNameA(nullptr, path, PLATFORM_MAX_PATH);
	if (len == 0 || len == PLATFORM_MAX_PATH)
		throw std::runtime_error("Failed to get executable path");
	return std::string(path);

#elif defined(__APPLE__)
	char path[PLATFORM_MAX_PATH];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) != 0)
		throw std::runtime_error("Failed to get executable path");
	char resolved[PLATFORM_MAX_PATH];
	if (realpath(path, resolved) == nullptr)
		throw std::runtime_error("Failed to resolve executable path");
	return std::string(resolved);

#elif defined(__linux__)
	char path[PLATFORM_MAX_PATH];
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
