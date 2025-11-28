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
    static int Run();
    static void Exit(int exitCode = 0);

    // Window management (static interface)
    static bool CreateAppWindow(const WindowSpec& spec);
    static void DestroyAppWindow();
    static void RestartAppWindow();
    static Window* GetWindow();
    static bool HasWindow();

    // Layer management (static template interface)
    // Usage: Application::PushLayer<MyLayer>(constructor_args...);
    template<typename T, typename... Args>
    static T* PushLayer(Args&&... args)
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& instance = Get();
        auto layer = std::make_unique<T>(std::forward<Args>(args)...);
        T* layerPtr = layer.get();

        layer->OnAttach();
        printf("Layer attached: %s\n", layer->GetName());
        instance.m_Layers.push_back(std::move(layer));

        return layerPtr;
    }

    // Remove layer by type
    template<typename T>
    static void RemoveLayer()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& instance = Get();
        auto it = std::find_if(instance.m_Layers.begin(), instance.m_Layers.end(),
            [](const std::unique_ptr<ApplicationLayer>& ptr) {
                return dynamic_cast<T*>(ptr.get()) != nullptr;
            });

        if (it != instance.m_Layers.end())
        {
            (*it)->OnDetach();
            printf("Layer removed: %s\n", (*it)->GetName());
            instance.m_Layers.erase(it);
        }
    }

    // Remove layer by pointer
    static void RemoveLayer(ApplicationLayer* layer);

    // Remove top layer
    static void PopLayer();

    // Get layer by type
    template<typename T>
    static T* GetLayerPtr()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        Application& instance = Get();
        for (auto& layer : instance.m_Layers)
        {
            if (T* casted = dynamic_cast<T*>(layer.get()))
            {
                return casted;
            }
        }
        return nullptr;
    }

    template<typename T>
    static T& GetLayer()
    {
        static_assert(std::is_base_of<ApplicationLayer, T>::value,
            "T must derive from ApplicationLayer");

        T* layer = GetLayerPtr<T>();
        if (!layer)
        {
            throw std::runtime_error("Layer of specified type not found");
        }
        return *layer;
	}

    // Event handling
    static void OnEvent(Event& event);

    // Application state
    static bool IsRunning();
    static int GetExitCode();
    static float GetDeltaTime();
    static float GetFPS();
    static float GetAvgFPS();
    static float FramesPerSecond();

private:
    Application();
    ~Application();

    // Singleton access (private)
    static Application& Get();

    // Internal implementation methods
    int Run_Internal();
    void Exit_Internal(int exitCode);
    bool CreateAppWindow_Internal(const WindowSpec& spec);
    void DestroyAppWindow_Internal();
    void RestartAppWindow_Internal();
    void PopLayer_Internal();
    void RemoveLayer_Internal(ApplicationLayer* layer);
    void OnEvent_Internal(Event& event);
    float GetFPS_Internal() const;
    float GetAvgFPS_Internal() const;
    float FramesPerSecond_Internal() const;

    void MainLoop();
    void UpdateLayers();
    void RenderLayers();

private:
    std::unique_ptr<Window> m_Window;
    std::vector<std::unique_ptr<ApplicationLayer>> m_Layers;

    bool m_Running;
    bool m_WindowRestartRequested;
    int m_ExitCode;
    float m_DeltaTime;

    // Average FPS calculation
    float m_AverageFPS;
    float m_AvgFPSTimeAccumulator;
    int m_AvgFPSFrameCount;

    WindowSpec m_PendingWindowSpec;
};

namespace rlx {
    template<typename T>
    inline bool RectangleContainsMouse(const rlx::Rectangle<T>& rect) {
        return rlx::RectangleContainsScaledMouse(rect, constant::BASE_WIDTH, constant::BASE_HEIGHT);
    }

    inline Vector2 GetMousePosition() {
        return rlx::GetScaledMousePosition(constant::BASE_WIDTH, constant::BASE_HEIGHT);
    }
}