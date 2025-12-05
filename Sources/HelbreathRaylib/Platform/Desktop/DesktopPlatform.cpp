#include "DesktopPlatform.h"

DesktopPlatform::DesktopPlatform() {
	// Initialize services if needed
}

ICursorManager& DesktopPlatform::getCursorManager() {
	return _cursorManager;
}

IInputTranslator& DesktopPlatform::getInputTranslator() {
	return _inputTranslator;
}

ITimerService& DesktopPlatform::getTimerService() {
	return _timerService;
}

IPlatformInfo& DesktopPlatform::getPlatformInfo() {
	return _platformInfo;
}

PlatformType DesktopPlatform::getPlatformType() const {
	return PlatformType::Desktop;
}
