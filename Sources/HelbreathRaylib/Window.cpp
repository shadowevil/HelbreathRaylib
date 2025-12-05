#include "Window.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "global_constants.h"

Window::Window(const WindowSpec& spec)
    : _title(spec.Title)
    , _width(spec.Width)
    , _height(spec.Height)
    , _target_fps(spec.TargetFPS)
    , _flags(spec.Flags)
    , _is_initialized(false)
    , _frames_per_second(0.0f)
    , _fps_time_accumulator(0.0f)
    , _fps_frame_count(0)
    , _last_width(spec.Width)
    , _last_height(spec.Height)
    , _last_focused(true)
    , _game_width(constant::BASE_WIDTH * constant::UPSCALE_FACTOR)
    , _game_height(constant::BASE_HEIGHT * constant::UPSCALE_FACTOR)
{
    // Apply configuration flags before window creation
    _apply_window_flags();

    // Create the window
    InitWindow(_width, _height, _title.c_str());

    if (!IsWindowReady())
    {
        // Window creation failed
        return;
    }

    // Don't set FPS limit on Emscripten - let it run at monitor refresh rate
#ifndef __EMSCRIPTEN__
    SetTargetFPS(_target_fps);
#endif

    // Initialize render target for upscaling if enabled
    if (HasFlag(_flags, WindowFlags::Upscaled))
    {
        _render_target = LoadRenderTexture(_game_width, _game_height);
        // Use bilinear filtering for smooth downscaling from high-res render target
        SetTextureFilter(_render_target.texture, TEXTURE_FILTER_BILINEAR);
    }

    _is_initialized = true;
}

Window::~Window()
{
    if (_is_initialized)
    {
        // Unload render target if it was created
        if (HasFlag(_flags, WindowFlags::Upscaled))
        {
            UnloadRenderTexture(_render_target);
        }

        CloseWindow();
        _is_initialized = false;
    }
}

void Window::_apply_window_flags()
{
    // Apply flags that need to be set BEFORE InitWindow
    if (HasFlag(_flags, WindowFlags::VSync))
    {
        SetConfigFlags(FLAG_VSYNC_HINT);
    }

    if (HasFlag(_flags, WindowFlags::MSAA_4X))
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
    }

    if (HasFlag(_flags, WindowFlags::Fullscreen))
    {
        SetConfigFlags(FLAG_FULLSCREEN_MODE);
    }

    if (HasFlag(_flags, WindowFlags::Resizable))
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    }

    if (HasFlag(_flags, WindowFlags::Borderless))
    {
        SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    }

    if (HasFlag(_flags, WindowFlags::AlwaysOnTop))
    {
        SetConfigFlags(FLAG_WINDOW_TOPMOST);
    }

    if (HasFlag(_flags, WindowFlags::Transparent))
    {
        SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    }

    if (HasFlag(_flags, WindowFlags::HighDPI))
    {
        SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    }
}

bool Window::is_open() const
{
    return _is_initialized && IsWindowReady();
}

bool Window::should_close() const
{
    return false;
}

float Window::get_delta_time() const
{
    return GetFrameTime();
}

float Window::get_fps() const
{
    return (float)::GetFPS();
}

int Window::get_frame_count() const
{
    // Raylib's GetFrameCount is a function, not a macro
    return GetFrameTime() > 0 ? static_cast<int>(1.0f / GetFrameTime()) : 0;
}

void Window::begin_frame()
{
    if (HasFlag(_flags, WindowFlags::Upscaled))
    {
        // Use existing upscale rendering system from raylib_include.h
        rlx::BeginUpscaleRender(_render_target, static_cast<float>(constant::UPSCALE_FACTOR));
        ClearBackground(BLACK);
    }
    else
    {
        // Render directly to screen
        BeginDrawing();
        ClearBackground(BLACK);
    }
}

void Window::end_frame()
{
    if (HasFlag(_flags, WindowFlags::Upscaled))
    {
        // Create lambda wrappers for event callbacks
        auto BeforeCallback = [this]() {
            if (_event_callback)
            {
                BeforeUpscaleEvent Event;
                _event_callback(Event);
            }
            if (_before_upscale_callback)
                _before_upscale_callback();
        };

        auto AfterCallback = [this]() {
            if (_event_callback)
            {
                AfterUpscaleEvent Event;
                _event_callback(Event);
            }
            if (_after_upscale_callback)
                _after_upscale_callback();
        };

        // Use existing upscale rendering system from raylib_include.h with event callbacks
        rlx::EndUpscaleRender(_render_target, BLACK, BeforeCallback, AfterCallback);
    }
    else
    {
        EndDrawing();
    }

    // Update custom FPS counter (frame accurate)
    float DeltaTime = get_delta_time();
    _fps_time_accumulator += DeltaTime;
    _fps_frame_count++;

    // Calculate FPS every second
    if (_fps_time_accumulator >= 1.0f)
    {
        _frames_per_second = static_cast<float>(_fps_frame_count) / _fps_time_accumulator;
        _fps_time_accumulator = 0.0f;
        _fps_frame_count = 0;
    }
}

void Window::close()
{
    if (_is_initialized)
    {
        CloseWindow();
        _is_initialized = false;
    }
}

void Window::set_title(const std::string& title)
{
    _title = title;
    if (_is_initialized)
    {
        SetWindowTitle(_title.c_str());
    }
}

void Window::set_size(int width, int height)
{
    _width = width;
    _height = height;
    if (_is_initialized)
    {
        SetWindowSize(_width, _height);
    }
}

void Window::toggle_fullscreen()
{
    if (_is_initialized)
    {
        ::ToggleFullscreen();

        // Update flag state
        if (IsWindowFullscreen())
        {
            _flags = _flags | WindowFlags::Fullscreen;
        }
        else
        {
            _flags = static_cast<WindowFlags>(
                static_cast<uint32_t>(_flags) & ~static_cast<uint32_t>(WindowFlags::Fullscreen)
                );
        }
    }
}

void Window::poll_events()
{
    if (!_is_initialized || !_event_callback)
        return;

    // Check for window resize
    int CurrentWidth = GetScreenWidth();
    int CurrentHeight = GetScreenHeight();
    if (CurrentWidth != _last_width || CurrentHeight != _last_height)
    {
        _last_width = CurrentWidth;
        _last_height = CurrentHeight;
        _width = CurrentWidth;
        _height = CurrentHeight;

        WindowResizeEvent Event(CurrentWidth, CurrentHeight);
        _event_callback(Event);
    }

    // Check for window focus changes
    bool CurrentlyFocused = IsWindowFocused();
    if (CurrentlyFocused != _last_focused)
    {
        _last_focused = CurrentlyFocused;

        if (CurrentlyFocused)
        {
            WindowFocusEvent Event;
            _event_callback(Event);
        }
        else
        {
            WindowLostFocusEvent Event;
            _event_callback(Event);
        }
    }

    // Note: WindowCloseEvent is checked in Application::MainLoop via should_close()
}
