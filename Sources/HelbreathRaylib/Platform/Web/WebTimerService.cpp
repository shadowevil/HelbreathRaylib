#include "WebTimerService.h"

WebTimerService::WebTimerService()
	: _nextTimerID(1) {
}

TimerID WebTimerService::scheduleRepeating(TimerCallback callback, float intervalSeconds) {
	TimerID id = _nextTimerID++;
	_timers.emplace(id, Timer(callback, intervalSeconds, true));
	return id;
}

void WebTimerService::scheduleOnce(TimerCallback callback, float delaySeconds) {
	TimerID id = _nextTimerID++;
	_timers.emplace(id, Timer(callback, delaySeconds, false));
}

void WebTimerService::cancel(TimerID id) {
	auto it = _timers.find(id);
	if (it != _timers.end()) {
		it->second.active = false;
		_timersToRemove.push_back(id);
	}
}

void WebTimerService::cancelAll() {
	_timers.clear();
	_timersToRemove.clear();
}

void WebTimerService::update(float deltaTime) {
	// Accumulate time and fire callbacks
	for (auto& [id, timer] : _timers) {
		if (!timer.active) continue;

		timer.accumulated += deltaTime;

		if (timer.accumulated >= timer.interval) {
			// Fire callback
			timer.callback();

			if (timer.repeating) {
				// Reset for next interval
				timer.accumulated -= timer.interval;
			} else {
				// Mark one-shot timer for removal
				timer.active = false;
				_timersToRemove.push_back(id);
			}
		}
	}

	// Remove inactive timers
	for (TimerID id : _timersToRemove) {
		_timers.erase(id);
	}
	_timersToRemove.clear();
}
