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
        : m_Width(width), m_Height(height) {
    }

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
    uint32_t m_Width;
    uint32_t m_Height;
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
        : m_X(x), m_Y(y)
    {
    }

    int GetX() const { return m_X; }
    int GetY() const { return m_Y; }

    EVENT_CLASS_TYPE(WindowMoved)
    EVENT_CLASS_CATEGORY(EventCategory::Window)

private:
    int m_X, m_Y;
};
