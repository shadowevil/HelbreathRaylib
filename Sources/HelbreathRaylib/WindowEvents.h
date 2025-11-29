#pragma once
#include "Event.h"

namespace EventCategory
{
    enum : uint32_t {
		Window = 1,
    };
}

namespace EventType
{
    enum : uint32_t {
        WindowClose         = 0,
        WindowResize        = 1,
        WindowFocus         = 2,
        WindowLostFocus     = 3,
        WindowMoved         = 4
    };
}

// Window close event
class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategory::Window)
};

// Window resize event
class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : _width(width), _height(height) {
    }

    uint32_t get_width() const { return _width; }
    uint32_t get_height() const { return _height; }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
    uint32_t _width;
    uint32_t _height;
};

// Window focus events
class WindowFocusEvent : public Event
{
public:
    WindowFocusEvent() = default;

    EVENT_CLASS_TYPE(WindowFocus)
    EVENT_CLASS_CATEGORY(EventCategory::Window)
};

class WindowLostFocusEvent : public Event
{
public:
    WindowLostFocusEvent() = default;

    EVENT_CLASS_TYPE(WindowLostFocus)
    EVENT_CLASS_CATEGORY(EventCategory::Window)
};

// Window moved event
class WindowMovedEvent : public Event
{
public:
    WindowMovedEvent(int x, int y)
        : _x(x), _y(y)
    {
    }

    int get_x() const { return _x; }
    int get_y() const { return _y; }

    EVENT_CLASS_TYPE(WindowMoved)
    EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
    int _x, _y;
};
