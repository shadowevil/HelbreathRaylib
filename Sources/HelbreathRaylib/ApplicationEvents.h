#pragma once
#include "Event.h"

namespace EventCategory
{
    // Application category already defined in WindowEvents.h
    enum : uint32_t
    {
        Application = 0
    };
}

namespace EventType
{
    enum ApplicationEventTypes : uint32_t {
        BeforeUpscale = 100,
        AfterUpscale = 101
    };
}

// Before upscale render event - called before rendering the upscaled target to screen
class BeforeUpscaleEvent : public Event
{
public:
    BeforeUpscaleEvent() = default;

    EVENT_CLASS_TYPE(BeforeUpscale)
    EVENT_CLASS_CATEGORY(EventCategory::Application)
};

// After upscale render event - called after rendering the upscaled target to screen
class AfterUpscaleEvent : public Event
{
public:
    AfterUpscaleEvent() = default;

    EVENT_CLASS_TYPE(AfterUpscale)
    EVENT_CLASS_CATEGORY(EventCategory::Application)
};
