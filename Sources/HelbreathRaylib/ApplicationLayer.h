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
        : _name(name), _enabled(true)
    {
    }

    virtual ~ApplicationLayer() = default;

    // Lifecycle hooks - override in derived classes
    virtual void on_attach() {}          // Called when layer is added to Application
    virtual void on_detach() {}          // Called when layer is removed
    virtual void on_update() {}          // Called every frame with delta time
    virtual void on_render() {}          // Called every frame for rendering
    virtual void on_event(Event& event) {}  // Called when an event occurs

    // Layer properties
    const char* get_name() const { return _name; }
    bool is_enabled() const { return _enabled; }
    void set_enabled(bool enabled) { _enabled = enabled; }

protected:
    const char* _name;
    bool _enabled;
};
