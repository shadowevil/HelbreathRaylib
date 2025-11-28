#pragma once
#include <string>
#include <functional>

class Event
{
public:
    virtual ~Event() = default;
    virtual uint32_t GetEventType() const = 0;
    virtual uint32_t GetCategoryFlags() const = 0;
    virtual const char* GetName() const = 0;
	std::string ToString() const { return GetName(); }

    bool IsInCategory(uint32_t category) const
    {
        return (GetCategoryFlags() & category) != 0;
    }

    bool IsHandled() const { return m_Handled; }
    void SetHandled(bool handled = true) { m_Handled = handled; }

protected:
    bool m_Handled = false;
};

class EventDispatcher
{
public:
    EventDispatcher(Event& event) : m_Event(event) {}

    template<typename T, typename F>
    bool Dispatch(const F& func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.SetHandled(func(static_cast<T&>(m_Event)));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

#define EVENT_CLASS_TYPE(type) \
    static uint32_t GetStaticType() { return EventType::type; } \
    virtual uint32_t GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual uint32_t GetCategoryFlags() const override { return category; }