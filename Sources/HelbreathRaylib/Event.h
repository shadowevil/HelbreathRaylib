#pragma once
#include <string>
#include <functional>

class Event
{
public:
    virtual ~Event() = default;
    virtual uint32_t get_event_type() const = 0;
    virtual uint32_t get_category_flags() const = 0;
    virtual const char* get_name() const = 0;
	std::string to_string() const { return get_name(); }

    bool is_in_category(uint32_t category) const
    {
        return (get_category_flags() & category) != 0;
    }

    bool is_handled() const { return _handled; }
    void set_handled(bool handled = true) { _handled = handled; }

protected:
    bool _handled = false;
};

class EventDispatcher
{
public:
    EventDispatcher(Event& event) : _event(event) {}

    template<typename T, typename F>
    bool dispatch(const F& func)
    {
        if (_event.get_event_type() == T::GET_STATIC_TYPE())
        {
            _event.set_handled(func(static_cast<T&>(_event)));
            return true;
        }
        return false;
    }

private:
    Event& _event;
};

#define EVENT_CLASS_TYPE(type) \
    static uint32_t GET_STATIC_TYPE() { return EventType::type; } \
    virtual uint32_t get_event_type() const override { return GET_STATIC_TYPE(); } \
    virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual uint32_t get_category_flags() const override { return category; }