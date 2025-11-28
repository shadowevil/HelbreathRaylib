#include "Window.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "global_constants.h"

Window::Window(const WindowSpec& spec)
    : m_Title(spec.Title)
    , m_Width(spec.Width)
    , m_Height(spec.Height)
    , m_TargetFPS(spec.TargetFPS)
    , m_Flags(spec.Flags)
    , m_IsInitialized(false)
    , m_FramesPerSecond(0.0f)
    , m_FPSTimeAccumulator(0.0f)
    , m_FPSFrameCount(0)
    , m_LastWidth(spec.Width)
    , m_LastHeight(spec.Height)
    , m_LastFocused(true)
    , m_GameWidth(constant::BASE_WIDTH * constant::UPSCALE_FACTOR)
    , m_GameHeight(constant::BASE_HEIGHT * constant::UPSCALE_FACTOR)
{
    // Apply configuration flags before window creation
    ApplyWindowFlags();

    // Create the window
    InitWindow(m_Width, m_Height, m_Title.c_str());

    if (!IsWindowReady())
    {
        // Window creation failed
        return;
    }

    SetTargetFPS(m_TargetFPS);

    // Initialize render target for upscaling if enabled
    if (HasFlag(m_Flags, WindowFlags::Upscaled))
    {
        m_RenderTarget = LoadRenderTexture(m_GameWidth, m_GameHeight);
        // Use bilinear filtering for smooth downscaling from high-res render target
        SetTextureFilter(m_RenderTarget.texture, TEXTURE_FILTER_BILINEAR);
    }

    m_IsInitialized = true;
}

Window::~Window()
{
    if (m_IsInitialized)
    {
        // Unload render target if it was created
        if (HasFlag(m_Flags, WindowFlags::Upscaled))
        {
            UnloadRenderTexture(m_RenderTarget);
        }

        CloseWindow();
        m_IsInitialized = false;
    }
}

void Window::ApplyWindowFlags()
{
    // Apply flags that need to be set BEFORE InitWindow
    if (HasFlag(m_Flags, WindowFlags::VSync))
    {
        SetConfigFlags(FLAG_VSYNC_HINT);
    }

    if (HasFlag(m_Flags, WindowFlags::MSAA_4X))
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
    }

    if (HasFlag(m_Flags, WindowFlags::Fullscreen))
    {
        SetConfigFlags(FLAG_FULLSCREEN_MODE);
    }

    if (HasFlag(m_Flags, WindowFlags::Resizable))
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    }

    if (HasFlag(m_Flags, WindowFlags::Borderless))
    {
        SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    }

    if (HasFlag(m_Flags, WindowFlags::AlwaysOnTop))
    {
        SetConfigFlags(FLAG_WINDOW_TOPMOST);
    }

    if (HasFlag(m_Flags, WindowFlags::Transparent))
    {
        SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    }

    if (HasFlag(m_Flags, WindowFlags::HighDPI))
    {
        SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    }
}

bool Window::IsOpen() const
{
    return m_IsInitialized && IsWindowReady();
}

bool Window::ShouldClose() const
{
    return WindowShouldClose();
}

float Window::GetDeltaTime() const
{
    return GetFrameTime();
}

float Window::GetFPS() const
{
    return (float)::GetFPS();
}

int Window::GetFrameCount() const
{
    // Raylib's GetFrameCount is a function, not a macro
    return GetFrameTime() > 0 ? static_cast<int>(1.0f / GetFrameTime()) : 0;
}

void Window::BeginFrame()
{
    if (HasFlag(m_Flags, WindowFlags::Upscaled))
    {
        // Use existing upscale rendering system from raylib_include.h
        rlx::BeginUpscaleRender(m_RenderTarget, static_cast<float>(constant::UPSCALE_FACTOR));
        ClearBackground(BLACK);
    }
    else
    {
        // Render directly to screen
        BeginDrawing();
        ClearBackground(BLACK);
    }
}

void Window::EndFrame()
{
    if (HasFlag(m_Flags, WindowFlags::Upscaled))
    {
        // Create lambda wrappers for event callbacks
        auto beforeCallback = [this]() {
            if (m_EventCallback)
            {
                BeforeUpscaleEvent event;
                m_EventCallback(event);
            }
            if (m_BeforeUpscaleCallback)
                m_BeforeUpscaleCallback();
        };

        auto afterCallback = [this]() {
            if (m_EventCallback)
            {
                AfterUpscaleEvent event;
                m_EventCallback(event);
            }
            if (m_AfterUpscaleCallback)
                m_AfterUpscaleCallback();
        };

        // Use existing upscale rendering system from raylib_include.h with event callbacks
        rlx::EndUpscaleRender(m_RenderTarget, BLACK, beforeCallback, afterCallback);
    }
    else
    {
        EndDrawing();
    }

    // Update custom FPS counter (frame accurate)
    float deltaTime = GetDeltaTime();
    m_FPSTimeAccumulator += deltaTime;
    m_FPSFrameCount++;

    // Calculate FPS every second
    if (m_FPSTimeAccumulator >= 1.0f)
    {
        m_FramesPerSecond = static_cast<float>(m_FPSFrameCount) / m_FPSTimeAccumulator;
        m_FPSTimeAccumulator = 0.0f;
        m_FPSFrameCount = 0;
    }
}

void Window::Close()
{
    if (m_IsInitialized)
    {
        CloseWindow();
        m_IsInitialized = false;
    }
}

void Window::SetTitle(const std::string& title)
{
    m_Title = title;
    if (m_IsInitialized)
    {
        SetWindowTitle(m_Title.c_str());
    }
}

void Window::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    if (m_IsInitialized)
    {
        SetWindowSize(m_Width, m_Height);
    }
}

void Window::ToggleFullscreen()
{
    if (m_IsInitialized)
    {
        ::ToggleFullscreen();

        // Update flag state
        if (IsWindowFullscreen())
        {
            m_Flags = m_Flags | WindowFlags::Fullscreen;
        }
        else
        {
            m_Flags = static_cast<WindowFlags>(
                static_cast<uint32_t>(m_Flags) & ~static_cast<uint32_t>(WindowFlags::Fullscreen)
                );
        }
    }
}

void Window::PollEvents()
{
    if (!m_IsInitialized || !m_EventCallback)
        return;

    // Check for window resize
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    if (currentWidth != m_LastWidth || currentHeight != m_LastHeight)
    {
        m_LastWidth = currentWidth;
        m_LastHeight = currentHeight;
        m_Width = currentWidth;
        m_Height = currentHeight;

        WindowResizeEvent event(currentWidth, currentHeight);
        m_EventCallback(event);
    }

    // Check for window focus changes
    bool currentlyFocused = IsWindowFocused();
    if (currentlyFocused != m_LastFocused)
    {
        m_LastFocused = currentlyFocused;

        if (currentlyFocused)
        {
            WindowFocusEvent event;
            m_EventCallback(event);
        }
        else
        {
            WindowLostFocusEvent event;
            m_EventCallback(event);
        }
    }

    // Note: WindowCloseEvent is checked in Application::MainLoop via ShouldClose()
}
