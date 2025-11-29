#pragma once
#include "raylib_include.h"
#include "Window.h"
#include "ApplicationLayer.h"
#include "SceneManager.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Application
{
public:
    // Delete copy/move
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Application lifecycle (static interface)
    static int run();
    static void exit(int exit_code = 0);

    // Window management (static interface)
    static bool create_app_window(const WindowSpec& spec);
    static void destroy_app_window();
    static void restart_app_window();
    static Window* get_window();
    static bool has_window();

    // Layer management (static template interface)
    // Usage: Application::push_layer<MyLayer>(constructor_args...);
    template<typename T, typename... Args>
    static T* push_layer(Args&&... args)
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& Instance = _get();
        auto Layer = std::make_unique<T>(std::forward<Args>(args)...);
        T* LayerPtr = Layer.get();

        Layer->on_attach();
        printf("Layer attached: %s\n", Layer->get_name());
        Instance._layers.push_back(std::move(Layer));

        return LayerPtr;
    }

    // Remove layer by type
    template<typename T>
    static void remove_layer()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& Instance = _get();
        auto It = std::find_if(Instance._layers.begin(), Instance._layers.end(),
            [](const std::unique_ptr<ApplicationLayer>& Ptr) {
                return dynamic_cast<T*>(Ptr.get()) != nullptr;
            });

        if (It != Instance._layers.end())
        {
            (*It)->on_detach();
            printf("Layer removed: %s\n", (*It)->get_name());
            Instance._layers.erase(It);
        }
    }

    // Remove layer by pointer
    static void remove_layer(ApplicationLayer* layer);

    // Remove top layer
    static void pop_layer();

    // Get layer by type
    template<typename T>
    static T* get_layer_ptr()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& Instance = _get();
        for (auto& Layer : Instance._layers)
        {
            if (T* Casted = dynamic_cast<T*>(Layer.get()))
            {
                return Casted;
            }
        }
        return nullptr;
    }

    template<typename T>
    static T& get_layer()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        T* Layer = get_layer_ptr<T>();
        if (!Layer)
        {
            throw std::runtime_error("Layer of specified type not found");
        }
        return *Layer;
	}

    // Event handling
    static void on_event(Event& event);

    // Application state
    static bool is_running();
    static int get_exit_code();
    static float get_delta_time();
    static float get_fps();
    static float get_avg_fps();
    static float frames_per_second();

private:
    Application();
    ~Application();

    // Singleton access (private)
    static Application& _get();

    // Internal implementation methods
    int _run_internal();
    void _exit_internal(int exit_code);
    bool _create_app_window_internal(const WindowSpec& spec);
    void _destroy_app_window_internal();
    void _restart_app_window_internal();
    void _pop_layer_internal();
    void _remove_layer_internal(ApplicationLayer* layer);
    void _on_event_internal(Event& event);
    float _get_fps_internal() const;
    float _get_avg_fps_internal() const;
    float _frames_per_second_internal() const;

    void _main_loop();
    void _update_layers();
    void _render_layers();

private:
    std::unique_ptr<Window> _window;
    std::vector<std::unique_ptr<ApplicationLayer>> _layers;

    bool _running;
    bool _window_restart_requested;
    int _exit_code;
    float _delta_time;

    // Average FPS calculation
    float _average_fps;
    float _avg_fps_time_accumulator;
    int _avg_fps_frame_count;

    WindowSpec _pending_window_spec;
};

namespace rlx {
    template<typename T>
    inline bool rectangle_contains_mouse(const rlx::Rectangle<T>& rect) {
        return rlx::RectangleContainsScaledMouse(rect, constant::BASE_WIDTH, constant::BASE_HEIGHT);
    }

    inline Vector2 get_mouse_position() {
        return rlx::GetScaledMousePosition(constant::BASE_WIDTH, constant::BASE_HEIGHT);
    }
}