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
    bool is_open() const;
    bool should_close() const;

    // Window properties
    int get_width() const { return _width; }
    int get_height() const { return _height; }
    const std::string& get_title() const { return _title; }
    WindowFlags get_flags() const { return _flags; }

    // Frame timing
    float get_delta_time() const;
    float get_fps() const;
    float frames_per_second() const { return _frames_per_second; }
    int get_frame_count() const;

    // Window control (called by Application only)
    void begin_frame();
    void end_frame();
    void close();

    // Window modification (triggers recreation)
    void set_title(const std::string& title);
    void set_size(int width, int height);
    void toggle_fullscreen();

    // Event callback
    void set_event_callback(const EventCallbackFn& callback) { _event_callback = callback; }

    // Upscale event callbacks
    void set_before_upscale_callback(const std::function<void()>& callback) { _before_upscale_callback = callback; }
    void set_after_upscale_callback(const std::function<void()>& callback) { _after_upscale_callback = callback; }

    // Poll and dispatch window events (called by Application)
    void poll_events();

private:
    void _apply_window_flags();

private:
    std::string _title;
    int _width;
    int _height;
    int _target_fps;
    WindowFlags _flags;
    bool _is_initialized;

    // Custom FPS counter (frame accurate)
    float _frames_per_second;
    float _fps_time_accumulator;
    int _fps_frame_count;

    // Event callback
    EventCallbackFn _event_callback;

    // State tracking for event generation
    int _last_width;
    int _last_height;
    bool _last_focused;

    // Render target for upscaling
    RenderTexture2D _render_target;
    int _game_width;
    int _game_height;

    // Upscale event callbacks
    std::function<void()> _before_upscale_callback;
    std::function<void()> _after_upscale_callback;
};