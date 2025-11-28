#include "Application.h"
#include <stdio.h>

Application::Application()
    : m_Running(false)
    , m_WindowRestartRequested(false)
    , m_ExitCode(0)
    , m_DeltaTime(0.0f)
    , m_AverageFPS(0.0f)
    , m_AvgFPSTimeAccumulator(0.0f)
    , m_AvgFPSFrameCount(0)
{
}

Application::~Application()
{
    // Clean up layers (smart pointers handle deletion automatically)
    for (auto& layer : m_Layers)
    {
        layer->OnDetach();
    }
    m_Layers.clear();

    // Clean up window
    DestroyAppWindow_Internal();
}

Application& Application::Get()
{
    static Application instance;
    return instance;
}

// Static wrapper methods
int Application::Run()
{
    return Get().Run_Internal();
}

void Application::Exit(int exitCode)
{
    Get().Exit_Internal(exitCode);
}

bool Application::CreateAppWindow(const WindowSpec& spec)
{
    return Get().CreateAppWindow_Internal(spec);
}

void Application::DestroyAppWindow()
{
    Get().DestroyAppWindow_Internal();
}

void Application::RestartAppWindow()
{
    Get().RestartAppWindow_Internal();
}

Window* Application::GetWindow()
{
    return Get().m_Window.get();
}

bool Application::HasWindow()
{
    return Get().m_Window != nullptr;
}

void Application::PopLayer()
{
    Get().PopLayer_Internal();
}

void Application::RemoveLayer(ApplicationLayer* layer)
{
    Get().RemoveLayer_Internal(layer);
}

void Application::OnEvent(Event& event)
{
    Get().OnEvent_Internal(event);
}

bool Application::IsRunning()
{
    return Get().m_Running;
}

int Application::GetExitCode()
{
    return Get().m_ExitCode;
}

float Application::GetDeltaTime()
{
    return Get().m_DeltaTime;
}

float Application::GetFPS()
{
    return Get().GetFPS_Internal();
}

float Application::GetAvgFPS()
{
    return Get().GetAvgFPS_Internal();
}

float Application::FramesPerSecond()
{
    return Get().FramesPerSecond_Internal();
}

// Internal implementation methods
bool Application::CreateAppWindow_Internal(const WindowSpec& spec)
{
    // Destroy existing window if any
    DestroyAppWindow_Internal();

    // Create new window
    m_Window = std::make_unique<Window>(spec);

    if (!m_Window->IsOpen())
    {
        printf("ERROR: Failed to create window!\n");
        m_Window.reset();
        return false;
    }
    else
    {
        // Set up event callback to dispatch events to layers
        m_Window->SetEventCallback([this](Event& e) {
            OnEvent_Internal(e);
            });

        printf("Window created: %dx%d - %s\n",
            spec.Width, spec.Height, spec.Title.c_str());
        return true;
    }
}

void Application::DestroyAppWindow_Internal()
{
    if (m_Window)
    {
        printf("Destroying window...\n");
        m_Window.reset();
    }
}

void Application::RestartAppWindow_Internal()
{
    if (m_Window)
    {
        // Save current window spec
        WindowSpec spec;
        spec.Title = m_Window->GetTitle();
        spec.Width = m_Window->GetWidth();
        spec.Height = m_Window->GetHeight();
        spec.Flags = m_Window->GetFlags();

        m_PendingWindowSpec = spec;
        m_WindowRestartRequested = true;

        printf("Window restart requested...\n");
    }
}

int Application::Run_Internal()
{
    if (m_Running)
    {
        printf("WARNING: Application is already running!\n");
        return m_ExitCode;
    }

    if (!m_Window)
    {
        printf("ERROR: No window created! Call CreateAppWindow() before Run()\n");
        return 1;
    }

    m_Running = true;
    m_ExitCode = 0;

    printf("Application starting main loop...\n");
    MainLoop();
    printf("Application exited with code: %d\n", m_ExitCode);

    return m_ExitCode;
}

void Application::Exit_Internal(int exitCode)
{
    printf("Application exit requested (code: %d)\n", exitCode);
    m_Running = false;
    m_ExitCode = exitCode;
}

void Application::MainLoop()
{
    while (m_Running)
    {
        // Handle window restart if requested
        if (m_WindowRestartRequested)
        {
            for (auto& layer : m_Layers)
            {
                layer->OnDetach();
            }

            m_WindowRestartRequested = false;
            CreateAppWindow_Internal(m_PendingWindowSpec);

            if (!m_Window)
            {
                // Window recreation failed - exit
                Exit_Internal(1);
                break;
            }

            for (auto& layer : m_Layers)
            {
                layer->OnAttach();
            }
        }

        // Check if window is still valid
        if (!m_Window || !m_Window->IsOpen())
        {
            printf("Window closed unexpectedly!\n");
            Exit_Internal(1);
            break;
        }

        // Check if user requested window close
        if (m_Window->ShouldClose())
        {
            printf("Window close requested by user\n");
            Exit_Internal(0);
            break;
        }

        // Poll and dispatch window events
        m_Window->PollEvents();

        // Calculate delta time
        m_DeltaTime = m_Window->GetDeltaTime();

        // Update average FPS calculation
        m_AvgFPSTimeAccumulator += m_DeltaTime;
        m_AvgFPSFrameCount++;

        // Calculate average FPS every second
        if (m_AvgFPSTimeAccumulator >= 1.0f)
        {
            m_AverageFPS = static_cast<float>(m_AvgFPSFrameCount) / m_AvgFPSTimeAccumulator;
            m_AvgFPSTimeAccumulator = 0.0f;
            m_AvgFPSFrameCount = 0;
        }

        // Begin frame
        m_Window->BeginFrame();

        // Update all layers
        UpdateLayers();

        // Render all layers
        RenderLayers();

        // End frame
        m_Window->EndFrame();
    }
}

void Application::UpdateLayers()
{
    for (auto& layer : m_Layers)
    {
        if (layer->IsEnabled())
        {
            layer->OnUpdate();
        }
    }
}

void Application::RenderLayers()
{
    for (auto& layer : m_Layers)
    {
        if (layer->IsEnabled())
        {
            layer->OnRender();
        }
    }
}

void Application::PopLayer_Internal()
{
    if (!m_Layers.empty())
    {
        auto& layer = m_Layers.back();
        layer->OnDetach();
        printf("Layer detached: %s\n", layer->GetName());
        m_Layers.pop_back();
        // unique_ptr automatically deletes layer
    }
}

void Application::RemoveLayer_Internal(ApplicationLayer* layer)
{
    auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
        [layer](const std::unique_ptr<ApplicationLayer>& ptr) {
            return ptr.get() == layer;
        });

    if (it != m_Layers.end())
    {
        (*it)->OnDetach();
        printf("Layer removed: %s\n", (*it)->GetName());
        m_Layers.erase(it);
        // unique_ptr automatically deletes layer
    }
}

void Application::OnEvent_Internal(Event& event)
{
    // Dispatch events to layers in reverse order (top layer gets first)
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
    {
        if (event.IsHandled())
            break;

        if ((*it)->IsEnabled())
        {
            (*it)->OnEvent(event);
        }
    }
}

float Application::GetFPS_Internal() const
{
    // Return our custom frame-accurate FPS counter
    return m_Window ? m_Window->FramesPerSecond() : 0.0f;
}

float Application::GetAvgFPS_Internal() const
{
    return m_AverageFPS;
}

float Application::FramesPerSecond_Internal() const
{
    return m_Window ? m_Window->FramesPerSecond() : 0.0f;
}