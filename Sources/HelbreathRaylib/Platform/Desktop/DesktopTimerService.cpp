#include "DesktopTimerService.h"
#include <chrono>

DesktopTimerService::DesktopTimerService()
	: _nextTimerID(1) {
}

DesktopTimerService::~DesktopTimerService() {
	cancelAll();
}

TimerID DesktopTimerService::scheduleRepeating(TimerCallback callback, float intervalSeconds) {
	TimerID id = _nextTimerID++;

	std::lock_guard<std::mutex> lock(_timersMutex);
	auto timer = std::make_unique<Timer>(callback, intervalSeconds, true);
	timer->thread = std::thread(&DesktopTimerService::runTimer, this, id, timer.get());
	_timers[id] = std::move(timer);

	return id;
}

void DesktopTimerService::scheduleOnce(TimerCallback callback, float delaySeconds) {
	TimerID id = _nextTimerID++;

	std::lock_guard<std::mutex> lock(_timersMutex);
	auto timer = std::make_unique<Timer>(callback, delaySeconds, false);
	timer->thread = std::thread(&DesktopTimerService::runTimer, this, id, timer.get());
	_timers[id] = std::move(timer);
}

void DesktopTimerService::cancel(TimerID id) {
	std::lock_guard<std::mutex> lock(_timersMutex);
	auto it = _timers.find(id);
	if (it != _timers.end()) {
		it->second->shouldRun.store(false);
		if (it->second->thread.joinable()) {
			it->second->thread.detach();  // Detach instead of join to avoid blocking
		}
		_timers.erase(it);
	}
}

void DesktopTimerService::cancelAll() {
	std::lock_guard<std::mutex> lock(_timersMutex);
	for (auto& [id, timer] : _timers) {
		timer->shouldRun.store(false);
		if (timer->thread.joinable()) {
			timer->thread.detach();
		}
	}
	_timers.clear();
}

void DesktopTimerService::update(float deltaTime) {
	// No-op on desktop - timers run in background threads
}

void DesktopTimerService::runTimer(TimerID id, Timer* timer) {
	do {
		auto duration = std::chrono::milliseconds(static_cast<long long>(timer->interval * 1000.0f));
		std::this_thread::sleep_for(duration);

		if (timer->shouldRun.load()) {
			timer->callback();
		}
	} while (timer->repeating && timer->shouldRun.load());

	// Clean up one-shot timers
	if (!timer->repeating) {
		std::lock_guard<std::mutex> lock(_timersMutex);
		_timers.erase(id);
	}
}
