#pragma once
#include "raylib_include.h"
#include <string>
#include <cstdint>
#include <functional>

// Window creation flags
enum class WindowFlags : uint32_t
{
    None = 0,
    Fullscreen = 1 << 0,
    Resizable = 1 << 1,
    VSync = 1 << 2,
    MSAA_4X = 1 << 3,
    Borderless = 1 << 4,
    AlwaysOnTop = 1 << 5,
    Transparent = 1 << 6,
    HighDPI = 1 << 7,
    Upscaled = 1 << 8,
};

// Bitwise operators for flags
inline WindowFlags operator|(WindowFlags a, WindowFlags b)
{
    return static_cast<WindowFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline WindowFlags operator&(WindowFlags a, WindowFlags b)
{
    return static_cast<WindowFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool HasFlag(WindowFlags flags, WindowFlags flag)
{
    return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
}

// Window specification
struct WindowSpec
{
    std::string Title = "Helbreath";
    int Width = 800;
    int Height = 600;
    int TargetFPS = 60;
    WindowFlags Flags = WindowFlags::VSync | WindowFlags::MSAA_4X;
};

// Forward declaration
class Event;

// Event callback function type
using EventCallbackFn = std::function<void(Event&)>;

// Window class - manages raylib window lifecycle
// NOTE: Only Application class should create/destroy Windows
class Window
{
public:
    Window(const WindowSpec& spec);
    ~Window();

    // Delete copy constructor/assignment
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Window state
    bool IsOpen() const;
    bool ShouldClose() const;

    // Window properties
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    const std::string& GetTitle() const { return m_Title; }
    WindowFlags GetFlags() const { return m_Flags; }

    // Frame timing
    float GetDeltaTime() const;
    float GetFPS() const;
    float FramesPerSecond() const { return m_FramesPerSecond; }
    int GetFrameCount() const;

    // Window control (called by Application only)
    void BeginFrame();
    void EndFrame();
    void Close();

    // Window modification (triggers recreation)
    void SetTitle(const std::string& title);
    void SetSize(int width, int height);
    void ToggleFullscreen();

    // Event callback
    void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

    // Upscale event callbacks
    void SetBeforeUpscaleCallback(const std::function<void()>& callback) { m_BeforeUpscaleCallback = callback; }
    void SetAfterUpscaleCallback(const std::function<void()>& callback) { m_AfterUpscaleCallback = callback; }

    // Poll and dispatch window events (called by Application)
    void PollEvents();

private:
    void ApplyWindowFlags();

private:
    std::string m_Title;
    int m_Width;
    int m_Height;
    int m_TargetFPS;
    WindowFlags m_Flags;
    bool m_IsInitialized;

    // Custom FPS counter (frame accurate)
    float m_FramesPerSecond;
    float m_FPSTimeAccumulator;
    int m_FPSFrameCount;

    // Event callback
    EventCallbackFn m_EventCallback;

    // State tracking for event generation
    int m_LastWidth;
    int m_LastHeight;
    bool m_LastFocused;

    // Render target for upscaling
    RenderTexture2D m_RenderTarget;
    int m_GameWidth;
    int m_GameHeight;

    // Upscale event callbacks
    std::function<void()> m_BeforeUpscaleCallback;
    std::function<void()> m_AfterUpscaleCallback;
};