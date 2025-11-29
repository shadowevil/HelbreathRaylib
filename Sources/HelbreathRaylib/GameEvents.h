#pragma once
#include "Event.h"

namespace EventCategory {
	enum : uint32_t {
		GAME = 2
	};
}

namespace EventType {
	enum : uint32_t {
		PERIODIC_TIMER = 200
	};
}

class PeriodicTimerEvent : public Event {
public:
	PeriodicTimerEvent() = default;

	EVENT_CLASS_TYPE(PERIODIC_TIMER)
	EVENT_CLASS_CATEGORY(EventCategory::GAME)
};