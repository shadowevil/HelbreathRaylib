#pragma once

#include <cmath>

#ifdef __EMSCRIPTEN__
extern "C"
{
#include "web_include/raylib.h"
#include "web_include/rlgl.h"
}
#include "web_include/raymath.h"
#else
extern "C"
{
#include "raylib.h"
#include "rlgl.h"
}
#include "raymath.h"
#endif

namespace raylib
{
	using ::Font;
	using ::Music;
	using ::Sound;

	using ::CloseWindow;
	using ::DrawRectangleLinesEx;
	using ::DrawRectangleRec;
	using ::DrawTextEx;
	using ::GetClipboardText;
	using ::GetFontDefault;
	using ::GetFPS;
	using ::HideCursor;
	using ::InitWindow;
	using ::IsWindowFullscreen;
	using ::IsWindowReady;
	using ::LoadFontEx;
	using ::LoadRenderTexture;
	using ::MeasureTextEx;
	using ::rlMultMatrixf;
	using ::rlPopMatrix;
	using ::rlPushMatrix;
	using ::rlTranslatef;
	using ::SetClipboardText;
	using ::SetTargetFPS;
	using ::SetWindowSize;
	using ::SetWindowTitle;
	using ::ShowCursor;
	using ::ToggleFullscreen;

	using ::IsKeyDown;
	using ::IsKeyPressed;
	using ::IsMouseButtonDown;
	using ::IsMouseButtonPressed;

	using ::FLAG_FULLSCREEN_MODE;
	using ::FLAG_MSAA_4X_HINT;
	using ::FLAG_VSYNC_HINT;
	using ::FLAG_WINDOW_HIGHDPI;
	using ::FLAG_WINDOW_RESIZABLE;
	using ::FLAG_WINDOW_TOPMOST;
	using ::FLAG_WINDOW_TRANSPARENT;
	using ::FLAG_WINDOW_UNDECORATED;

	using ::KEY_A;
	using ::KEY_C;
	using ::KEY_ENTER;
	using ::KEY_ESCAPE;
	using ::KEY_LEFT_SHIFT;
	using ::KEY_RIGHT_CONTROL;
	using ::KEY_RIGHT_SHIFT;
	using ::KEY_TAB;
	using ::KEY_V;
	using ::KEY_X;

	using ::GetCharPressed;
	using ::IsKeyPressedRepeat;
	using ::KEY_BACKSPACE;
	using ::KEY_DELETE;
	using ::KEY_END;
	using ::KEY_HOME;
	using ::KEY_KP_ENTER;
	using ::KEY_LEFT;
	using ::KEY_LEFT_CONTROL;
	using ::KEY_RIGHT;

	using ::MOUSE_BUTTON_LEFT;
	using ::MOUSE_BUTTON_MIDDLE;
	using ::MOUSE_BUTTON_RIGHT;
	using ::TEXTURE_FILTER_BILINEAR;

	using ::Color;
	using ::DrawRectangle;
	using ::GetFrameTime;
	using ::GetMousePosition;
	using ::GetScreenHeight;
	using ::GetScreenWidth;
	using ::GetTime;
	using ::Image;
	using ::IsTextureValid;
	using ::LoadImageFromMemory;
	using ::LoadTextureFromImage;
	using ::LOG_ERROR;
	using ::Rectangle;
	using ::RenderTexture2D;
	using ::SetTextureFilter;
	using ::SetTraceLogLevel;
	using ::Texture2D;
	using ::TEXTURE_FILTER_POINT;
	using ::UnloadImage;
	using ::UnloadTexture;
	using ::Vector2;

	using ::BeginDrawing;
	using ::BeginMode2D;
	using ::BeginTextureMode;
	using ::Camera2D;
	using ::ClearBackground;
	using ::DrawTexturePro;
	using ::EndDrawing;
	using ::EndMode2D;
	using ::EndTextureMode;

	using ::AudioStream;
	using ::BeginScissorMode;
	using ::CloseAudioDevice;
	using ::EndScissorMode;
	using ::FileExists;
	using ::GetMusicTimeLength;
	using ::GetMusicTimePlayed;
	using ::GetRandomValue;
	using ::InitAudioDevice;
	using ::IsAudioDeviceReady;
	using ::IsSoundPlaying;
	using ::IsWindowFocused;
	using ::LoadMusicStream;
	using ::LoadSound;
	using ::LoadSoundAlias;
	using ::PlayMusicStream;
	using ::PlaySound;
	using ::rlDisableScissorTest;
	using ::rlDrawRenderBatchActive;
	using ::SeekMusicStream;
	using ::SetMusicVolume;
	using ::SetSoundPan;
	using ::SetSoundVolume;
	using ::StopMusicStream;
	using ::StopSound;
	using ::UnloadFont;
	using ::UnloadMusicStream;
	using ::UnloadSound;
	using ::UnloadSoundAlias;
	using ::UpdateMusicStream;
}

#include "path_utils.h"
#include "color_types.h"
#include "geometry_types.h"
#include "file_utils.h"
#include "render_utils.h"
#include "string_utils.h"
