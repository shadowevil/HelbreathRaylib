#include "DesktopPlatformInfo.h"

PlatformType DesktopPlatformInfo::getType() const {
	return PlatformType::Desktop;
}

const char* DesktopPlatformInfo::getName() const {
	return "Desktop";
}

PlatformCapabilities DesktopPlatformInfo::getCapabilities() const {
	PlatformCapabilities caps;
	caps.supportsThreading = true;
	caps.supportsPointerLock = true;
	caps.supportsFullscreen = true;
	caps.maxTextureSize = 8192;  // Typical modern GPU limit
	caps.defaultWindowWidth = 1920;
	caps.defaultWindowHeight = 1080;
	return caps;
}
