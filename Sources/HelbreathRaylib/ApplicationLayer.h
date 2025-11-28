#pragma once
#include "Event.h"

// Forward declarations
class Application;

// ApplicationLayer - base class for game logic, UI, etc.
// Derive from this to create custom layers (GameLayer, MenuLayer, etc.)
class ApplicationLayer
{
public:
    ApplicationLayer(const char* name = "Layer")
        : m_Name(name), m_Enabled(true)
    {
    }

    virtual ~ApplicationLayer() = default;

    // Lifecycle hooks - override in derived classes
    virtual void OnAttach() {}          // Called when layer is added to Application
    virtual void OnDetach() {}          // Called when layer is removed
    virtual void OnUpdate() {}          // Called every frame with delta time
    virtual void OnRender() {}          // Called every frame for rendering
    virtual void OnEvent(Event& event) {}  // Called when an event occurs

    // Layer properties
    const char* GetName() const { return m_Name; }
    bool IsEnabled() const { return m_Enabled; }
    void SetEnabled(bool enabled) { m_Enabled = enabled; }

protected:
    const char* m_Name;
    bool m_Enabled;
};
