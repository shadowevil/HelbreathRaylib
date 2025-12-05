#pragma once
#include "../ITimerService.h"
#include <thread>
#include <atomic>
#include <map>
#include <mutex>

/**
 * Desktop implementation of timer service.
 * Uses std::thread for background periodic tasks.
 */
class DesktopTimerService : public ITimerService {
public:
	DesktopTimerService();
	~DesktopTimerService() override;

	TimerID scheduleRepeating(TimerCallback callback, float intervalSeconds) override;
	void scheduleOnce(TimerCallback callback, float delaySeconds) override;
	void cancel(TimerID id) override;
	void cancelAll() override;
	void update(float deltaTime) override;

private:
	struct Timer {
		TimerCallback callback;
		float interval;
		bool repeating;
		std::thread thread;
		std::atomic<bool> shouldRun;

		Timer(TimerCallback cb, float iv, bool rep)
			: callback(cb), interval(iv), repeating(rep), shouldRun(true) {}
	};

	std::atomic<TimerID> _nextTimerID;
	std::map<TimerID, std::unique_ptr<Timer>> _timers;
	std::mutex _timersMutex;

	void runTimer(TimerID id, Timer* timer);
};
