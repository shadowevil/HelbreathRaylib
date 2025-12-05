#pragma once
#include "../ITimerService.h"
#include <map>
#include <vector>

/**
 * Web implementation of timer service.
 * Uses frame-based timing (no threads in browser).
 * Accumulates delta time and fires callbacks when interval is reached.
 */
class WebTimerService : public ITimerService {
public:
	WebTimerService();
	~WebTimerService() override = default;

	TimerID scheduleRepeating(TimerCallback callback, float intervalSeconds) override;
	void scheduleOnce(TimerCallback callback, float delaySeconds) override;
	void cancel(TimerID id) override;
	void cancelAll() override;
	void update(float deltaTime) override;

private:
	struct Timer {
		TimerCallback callback;
		float interval;
		float accumulated;
		bool repeating;
		bool active;

		Timer(TimerCallback cb, float iv, bool rep)
			: callback(cb), interval(iv), accumulated(0.0f), repeating(rep), active(true) {}
	};

	TimerID _nextTimerID;
	std::map<TimerID, Timer> _timers;
	std::vector<TimerID> _timersToRemove;  // Track timers to remove after update
};
