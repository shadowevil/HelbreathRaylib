#pragma once
#include <functional>
#include <cstdint>

using TimerID = uint32_t;
using TimerCallback = std::function<void()>;

/**
 * Timer and periodic event abstraction.
 * Handles platform-specific timing mechanisms.
 *
 * Desktop: Uses std::thread with sleep for background periodic tasks
 * Web: Frame-based timing with delta accumulation (no threads)
 */
class ITimerService {
public:
	virtual ~ITimerService() = default;

	// Schedule a repeating callback at fixed intervals
	// Returns TimerID that can be used to cancel the timer
	virtual TimerID scheduleRepeating(TimerCallback callback, float intervalSeconds) = 0;

	// Schedule a one-time callback after a delay
	virtual void scheduleOnce(TimerCallback callback, float delaySeconds) = 0;

	// Cancel a specific timer
	virtual void cancel(TimerID id) = 0;

	// Cancel all timers
	virtual void cancelAll() = 0;

	// Update timers (called once per frame)
	// Desktop: may be no-op if using threads
	// Web: accumulates time and fires callbacks when interval reached
	virtual void update(float deltaTime) = 0;
};
