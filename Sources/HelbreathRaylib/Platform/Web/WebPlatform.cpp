#include "WebPlatform.h"

WebPlatform::WebPlatform() {
	// Initialize services if needed
}

ICursorManager& WebPlatform::getCursorManager() {
	return _cursorManager;
}

IInputTranslator& WebPlatform::getInputTranslator() {
	return _inputTranslator;
}

ITimerService& WebPlatform::getTimerService() {
	return _timerService;
}

IPlatformInfo& WebPlatform::getPlatformInfo() {
	return _platformInfo;
}

PlatformType WebPlatform::getPlatformType() const {
	return PlatformType::Web;
}
