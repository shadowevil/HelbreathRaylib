#pragma once
#include "../IPlatformServices.h"
#include "DesktopCursorManager.h"
#include "DesktopInputTranslator.h"
#include "DesktopTimerService.h"
#include "DesktopPlatformInfo.h"
#include <memory>

/**
 * Desktop platform implementation.
 * Aggregates all desktop-specific service implementations.
 */
class DesktopPlatform : public IPlatformServices {
public:
	DesktopPlatform();
	~DesktopPlatform() override = default;

	ICursorManager& getCursorManager() override;
	IInputTranslator& getInputTranslator() override;
	ITimerService& getTimerService() override;
	IPlatformInfo& getPlatformInfo() override;
	PlatformType getPlatformType() const override;

private:
	DesktopCursorManager _cursorManager;
	DesktopInputTranslator _inputTranslator;
	DesktopTimerService _timerService;
	DesktopPlatformInfo _platformInfo;
};
