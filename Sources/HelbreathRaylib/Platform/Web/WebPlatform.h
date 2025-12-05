#pragma once
#include "../IPlatformServices.h"
#include "WebCursorManager.h"
#include "WebInputTranslator.h"
#include "WebTimerService.h"
#include "WebPlatformInfo.h"
#include <memory>

/**
 * Web platform implementation.
 * Aggregates all web-specific service implementations.
 */
class WebPlatform : public IPlatformServices {
public:
	WebPlatform();
	~WebPlatform() override = default;

	ICursorManager& getCursorManager() override;
	IInputTranslator& getInputTranslator() override;
	ITimerService& getTimerService() override;
	IPlatformInfo& getPlatformInfo() override;
	PlatformType getPlatformType() const override;

private:
	WebCursorManager _cursorManager;
	WebInputTranslator _inputTranslator;
	WebTimerService _timerService;
	WebPlatformInfo _platformInfo;
};
