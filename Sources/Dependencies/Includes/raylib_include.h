#pragma once
// Include Windows API wrapper (only on Windows)
#include "windows_wrapper.h"

// Include raylib with guards to prevent multiple includes
#ifndef _RAYLIB_CPP_INCLUDE_
#define _RAYLIB_CPP_INCLUDE_
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
#endif

// Include abstracted utility headers
// All implementation logic has been moved to separate, focused header files
#include "path_utils.h"      // Path manipulation (get_executable_path, path_combine, etc.)
#include "color_types.h"      // RGB/RGBA color structures
#include "geometry_types.h"   // Rectangle, Box, Padding, Margin structures
#include "file_utils.h"       // File and Directory namespace utilities
#include "render_utils.h"     // Upscaling, cursor lock, mouse position utilities
#include "string_utils.h"     // String manipulation utilities (to_lower, etc.)

// Raylib wrapper namespace - exposes only the functions we actually use
// This provides a controlled interface and hides the full raylib API
namespace raylib {
	// Type aliases for raylib structs
	using Vector2 = ::Vector2;
	using Vector3 = ::Vector3;
	using Vector4 = ::Vector4;
	using Color = ::Color;
	using Rectangle = ::Rectangle;
	using Image = ::Image;
	using Texture = ::Texture;
	using Texture2D = ::Texture2D;
	using RenderTexture = ::RenderTexture;
	using RenderTexture2D = ::RenderTexture2D;
	using NPatchInfo = ::NPatchInfo;
	using GlyphInfo = ::GlyphInfo;
	using Font = ::Font;
	using Camera2D = ::Camera2D;
	using Camera3D = ::Camera3D;
	using Mesh = ::Mesh;
	using Shader = ::Shader;
	using MaterialMap = ::MaterialMap;
	using Material = ::Material;
	using Transform = ::Transform;
	using BoneInfo = ::BoneInfo;
	using Model = ::Model;
	using ModelAnimation = ::ModelAnimation;
	using Ray = ::Ray;
	using RayCollision = ::RayCollision;
	using BoundingBox = ::BoundingBox;
	using Wave = ::Wave;
	using AudioStream = ::AudioStream;
	using Sound = ::Sound;
	using Music = ::Music;

	// Window Management Functions
	inline void InitWindow(int width, int height, const char* title) { ::InitWindow(width, height, title); }
	inline void CloseWindow() { ::CloseWindow(); }
	inline bool IsWindowReady() { return ::IsWindowReady(); }
	inline bool IsWindowFullscreen() { return ::IsWindowFullscreen(); }
	inline bool IsWindowFocused() { return ::IsWindowFocused(); }
	inline void ToggleFullscreen() { ::ToggleFullscreen(); }
	inline void SetWindowTitle(const char* title) { ::SetWindowTitle(title); }
	inline void SetWindowSize(int width, int height) { ::SetWindowSize(width, height); }
	inline void SetConfigFlags(unsigned int flags) { ::SetConfigFlags(flags); }
	inline void SetTargetFPS(int fps) { ::SetTargetFPS(fps); }
	inline void SetTraceLogLevel(int logLevel) { ::SetTraceLogLevel(logLevel); }

	// Drawing & Rendering Functions
	inline void BeginDrawing() { ::BeginDrawing(); }
	inline void EndDrawing() { ::EndDrawing(); }
	inline void ClearBackground(Color color) { ::ClearBackground(color); }
	inline void DrawRectangle(int posX, int posY, int width, int height, Color color) { ::DrawRectangle(posX, posY, width, height, color); }
	inline void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color) { ::DrawRectangleLinesEx(rec, lineThick, color); }
	inline void DrawTextEx(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint) { ::DrawTextEx(font, text, position, fontSize, spacing, tint); }
	inline void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint) { ::DrawTexturePro(texture, source, dest, origin, rotation, tint); }
	inline void BeginMode2D(Camera2D camera) { ::BeginMode2D(camera); }
	inline void EndMode2D() { ::EndMode2D(); }
	inline void BeginTextureMode(RenderTexture2D target) { ::BeginTextureMode(target); }
	inline void EndTextureMode() { ::EndTextureMode(); }

	// Texture & Image Management Functions
	inline Texture2D LoadTextureFromImage(Image image) { return ::LoadTextureFromImage(image); }
	inline RenderTexture2D LoadRenderTexture(int width, int height) { return ::LoadRenderTexture(width, height); }
	inline void UnloadTexture(Texture2D texture) { ::UnloadTexture(texture); }
	inline void UnloadRenderTexture(RenderTexture2D target) { ::UnloadRenderTexture(target); }
	inline Image LoadImageFromMemory(const char* fileType, const unsigned char* fileData, int dataSize) { return ::LoadImageFromMemory(fileType, fileData, dataSize); }
	inline void UnloadImage(Image image) { ::UnloadImage(image); }
	inline void SetTextureFilter(Texture2D texture, int filter) { ::SetTextureFilter(texture, filter); }

	// Font & Text Functions
	inline Font GetFontDefault() { return ::GetFontDefault(); }
	inline Font LoadFontEx(const char* fileName, int fontSize, int* codepoints, int codepointCount) { return ::LoadFontEx(fileName, fontSize, codepoints, codepointCount); }
	inline void UnloadFont(Font font) { ::UnloadFont(font); }
	inline Vector2 MeasureTextEx(Font font, const char* text, float fontSize, float spacing) { return ::MeasureTextEx(font, text, fontSize, spacing); }

	// Audio - Sound Functions
	inline Sound LoadSound(const char* fileName) { return ::LoadSound(fileName); }
	inline Sound LoadSoundAlias(Sound source) { return ::LoadSoundAlias(source); }
	inline void UnloadSound(Sound sound) { ::UnloadSound(sound); }
	inline void UnloadSoundAlias(Sound alias) { ::UnloadSoundAlias(alias); }
	inline void PlaySound(Sound sound) { ::PlaySound(sound); }
	inline void StopSound(Sound sound) { ::StopSound(sound); }
	inline bool IsSoundPlaying(Sound sound) { return ::IsSoundPlaying(sound); }
	inline void SetSoundVolume(Sound sound, float volume) { ::SetSoundVolume(sound, volume); }
	inline void SetSoundPan(Sound sound, float pan) { ::SetSoundPan(sound, pan); }

	// Audio - Music Functions
	inline Music LoadMusicStream(const char* fileName) { return ::LoadMusicStream(fileName); }
	inline void UnloadMusicStream(Music music) { ::UnloadMusicStream(music); }
	inline void PlayMusicStream(Music music) { ::PlayMusicStream(music); }
	inline void StopMusicStream(Music music) { ::StopMusicStream(music); }
	inline void UpdateMusicStream(Music music) { ::UpdateMusicStream(music); }
	inline void SeekMusicStream(Music music, float position) { ::SeekMusicStream(music, position); }
	inline void SetMusicVolume(Music music, float volume) { ::SetMusicVolume(music, volume); }
	inline float GetMusicTimeLength(Music music) { return ::GetMusicTimeLength(music); }
	inline float GetMusicTimePlayed(Music music) { return ::GetMusicTimePlayed(music); }

	// Audio - Device Functions
	inline void InitAudioDevice() { ::InitAudioDevice(); }
	inline void CloseAudioDevice() { ::CloseAudioDevice(); }
	inline bool IsAudioDeviceReady() { return ::IsAudioDeviceReady(); }

	// Input - Mouse Functions
	inline Vector2 GetMousePosition() { return ::GetMousePosition(); }
	inline bool IsMouseButtonPressed(int button) { return ::IsMouseButtonPressed(button); }
	inline bool IsMouseButtonDown(int button) { return ::IsMouseButtonDown(button); }
	inline void ShowCursor() { ::ShowCursor(); }
	inline void HideCursor() { ::HideCursor(); }

	// Input - Keyboard Functions
	inline bool IsKeyPressed(int key) { return ::IsKeyPressed(key); }
	inline bool IsKeyDown(int key) { return ::IsKeyDown(key); }

	// Screen & Timing Functions
	inline int GetScreenWidth() { return ::GetScreenWidth(); }
	inline int GetScreenHeight() { return ::GetScreenHeight(); }
	inline float GetFrameTime() { return ::GetFrameTime(); }
	inline int GetFPS() { return ::GetFPS(); }
	inline double GetTime() { return ::GetTime(); }
	inline int GetRandomValue(int min, int max) { return ::GetRandomValue(min, max); }
}
