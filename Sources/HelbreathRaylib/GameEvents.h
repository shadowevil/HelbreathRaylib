#pragma once
#include "Event.h"

namespace EventCategory {
	enum : uint32_t {
		Game = 2
	};
}

namespace EventType {
	enum : uint32_t {
		PerodicTimer = 200
	};
}

class PeriodicTimerEvent : public Event {
public:
	PeriodicTimerEvent() = default;

	EVENT_CLASS_TYPE(PerodicTimer)
	EVENT_CLASS_CATEGORY(EventCategory::Game)
};