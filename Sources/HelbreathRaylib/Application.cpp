#include "Application.h"
#include "Platform/PlatformFactory.h"
#include "Platform/IInputTranslator.h"
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Application::Application()
    : _running(false)
    , _window_restart_requested(false)
    , _exit_code(0)
    , _delta_time(0.0f)
    , _average_fps(0.0f)
    , _avg_fps_time_accumulator(0.0f)
    , _avg_fps_frame_count(0)
{
}

Application::~Application()
{
    // Clean up layers (smart pointers handle deletion automatically)
    for (auto& Layer : _layers)
    {
        Layer->on_detach();
    }
    _layers.clear();

    // Clean up window
    _destroy_app_window_internal();
}

Application& Application::_get()
{
    static Application Instance;
    return Instance;
}

// Static wrapper methods
int Application::run()
{
    return _get()._run_internal();
}

void Application::exit(int exit_code)
{
    _get()._exit_internal(exit_code);
}

bool Application::create_app_window(const WindowSpec& spec, std::unique_ptr<IPlatformServices> platform)
{
    return _get()._create_app_window_internal(spec, std::move(platform));
}

IPlatformServices* Application::get_platform()
{
    return _get()._platform.get();
}

void Application::destroy_app_window()
{
    _get()._destroy_app_window_internal();
}

void Application::restart_app_window()
{
    _get()._restart_app_window_internal();
}

Window* Application::get_window()
{
    return _get()._window.get();
}

bool Application::has_window()
{
    return _get()._window != nullptr;
}

void Application::pop_layer()
{
    _get()._pop_layer_internal();
}

void Application::remove_layer(ApplicationLayer* layer)
{
    _get()._remove_layer_internal(layer);
}

void Application::on_event(Event& event)
{
    _get()._on_event_internal(event);
}

bool Application::is_running()
{
    return _get()._running;
}

int Application::get_exit_code()
{
    return _get()._exit_code;
}

float Application::get_delta_time()
{
    return _get()._delta_time;
}

float Application::get_fps()
{
    return _get()._get_fps_internal();
}

float Application::get_avg_fps()
{
    return _get()._get_avg_fps_internal();
}

float Application::frames_per_second()
{
    return _get()._frames_per_second_internal();
}

// Internal implementation methods
bool Application::_create_app_window_internal(const WindowSpec& spec, std::unique_ptr<IPlatformServices> platform)
{
    // Destroy existing window if any
    _destroy_app_window_internal();

    // Store platform services (create default using PlatformFactory if not provided)
    if (platform) {
        _platform = std::move(platform);
    } else if (!_platform) {
        // Use PlatformFactory to create appropriate platform
        _platform = PlatformFactory::create();
    }

    // Create new window
    _window = std::make_unique<Window>(spec);

    if (!_window->is_open())
    {
        printf("ERROR: Failed to create window!\n");
        _window.reset();
        return false;
    }
    else
    {
        // Set up event callback to dispatch events to layers
        _window->set_event_callback([this](Event& e) {
            _on_event_internal(e);
            });

        printf("Window created: %dx%d - %s\n",
            spec.Width, spec.Height, spec.Title.c_str());
        return true;
    }
}

void Application::_destroy_app_window_internal()
{
    if (_window)
    {
        printf("Destroying window...\n");
        _window.reset();
    }
}

void Application::_restart_app_window_internal()
{
    if (_window)
    {
        // Save current window spec
        WindowSpec Spec;
        Spec.Title = _window->get_title();
        Spec.Width = _window->get_width();
        Spec.Height = _window->get_height();
        Spec.Flags = _window->get_flags();

        _pending_window_spec = Spec;
        _window_restart_requested = true;

        printf("Window restart requested...\n");
    }
}

int Application::_run_internal()
{
    if (_running)
    {
        printf("WARNING: Application is already running!\n");
        return _exit_code;
    }

    if (!_window)
    {
        printf("ERROR: No window created! Call create_app_window() before run()\n");
        return 1;
    }

    _running = true;
    _exit_code = 0;

    printf("Application starting main loop...\n");
    _main_loop();
    printf("Application exited with code: %d\n", _exit_code);

    return _exit_code;
}

void Application::_exit_internal(int exit_code)
{
    printf("Application exit requested (code: %d)\n", exit_code);
    _running = false;
    _exit_code = exit_code;
}

void Application::_main_loop()
{
#ifdef __EMSCRIPTEN__
    // For Emscripten, use emscripten_set_main_loop instead of while loop
    // This allows the browser to handle events and prevents freezing
    emscripten_set_main_loop(_emscripten_loop_callback, 0, 1);
#else
    // Native builds use traditional while loop
    while (_running)
    {
        _main_loop_iteration();
    }
#endif
}

void Application::_update_layers()
{
    for (auto& Layer : _layers)
    {
        if (Layer->is_enabled())
        {
            Layer->on_update();
        }
    }
}

void Application::_render_layers()
{
    for (auto& Layer : _layers)
    {
        if (Layer->is_enabled())
        {
            Layer->on_render();
        }
    }
}

void Application::_pop_layer_internal()
{
    if (!_layers.empty())
    {
        auto& Layer = _layers.back();
        Layer->on_detach();
        printf("Layer detached: %s\n", Layer->get_name());
        _layers.pop_back();
        // unique_ptr automatically deletes layer
    }
}

void Application::_remove_layer_internal(ApplicationLayer* layer)
{
    auto It = std::find_if(_layers.begin(), _layers.end(),
        [layer](const std::unique_ptr<ApplicationLayer>& Ptr) {
            return Ptr.get() == layer;
        });

    if (It != _layers.end())
    {
        (*It)->on_detach();
        printf("Layer removed: %s\n", (*It)->get_name());
        _layers.erase(It);
        // unique_ptr automatically deletes layer
    }
}

void Application::_on_event_internal(Event& event)
{
    // Dispatch events to layers in reverse order (top layer gets first)
    for (auto It = _layers.rbegin(); It != _layers.rend(); ++It)
    {
        if (event.is_handled())
            break;

        if ((*It)->is_enabled())
        {
            (*It)->on_event(event);
        }
    }
}

float Application::_get_fps_internal() const
{
    // Return our custom frame-accurate FPS counter
    return _window ? _window->frames_per_second() : 0.0f;
}

float Application::_get_avg_fps_internal() const
{
    return _average_fps;
}

float Application::_frames_per_second_internal() const
{
    return _window ? _window->frames_per_second() : 0.0f;
}
#ifdef __EMSCRIPTEN__
// Static callback for Emscripten main loop
void Application::_emscripten_loop_callback()
{
    Application& app = _get();
    app._main_loop_iteration();
}
#endif

// Single frame iteration - extracted from _main_loop for Emscripten
void Application::_main_loop_iteration()
{
    // Handle window restart if requested
    if (_window_restart_requested)
    {
        for (auto& Layer : _layers)
        {
            Layer->on_detach();
        }

        _window_restart_requested = false;
        _create_app_window_internal(_pending_window_spec, nullptr);

        if (!_window)
        {
            // Window recreation failed - exit
            _exit_internal(1);
            return;
        }

        for (auto& Layer : _layers)
        {
            Layer->on_attach();
        }
    }

    // Check if window is still valid
    if (!_window || !_window->is_open())
    {
        printf("Window closed unexpectedly!\n");
        _exit_internal(1);
        return;
    }

    // Check if user requested window close
    if (_window->should_close())
    {
        printf("Window close requested by user\n");
        _exit_internal(0);
        return;
    }

    // Poll and dispatch window events
    _window->poll_events();

    // Process platform-specific input (e.g., block browser shortcuts on web)
    if (_platform) {
        _platform->getInputTranslator().processInput();
    }

    // Calculate delta time
    _delta_time = _window->get_delta_time();

    // Update average FPS calculation
    _avg_fps_time_accumulator += _delta_time;
    _avg_fps_frame_count++;

    // Calculate average FPS every second
    if (_avg_fps_time_accumulator >= 1.0f)
    {
        _average_fps = static_cast<float>(_avg_fps_frame_count) / _avg_fps_time_accumulator;
        _avg_fps_time_accumulator = 0.0f;
        _avg_fps_frame_count = 0;
    }

    // Begin frame
    _window->begin_frame();

    // Update all layers
    _update_layers();

    // Render all layers
    _render_layers();

    // End frame
    _window->end_frame();
}
