#include "WebPlatformInfo.h"

PlatformType WebPlatformInfo::getType() const {
	return PlatformType::Web;
}

const char* WebPlatformInfo::getName() const {
	return "Web";
}

PlatformCapabilities WebPlatformInfo::getCapabilities() const {
	PlatformCapabilities caps;
	caps.supportsThreading = false;  // No std::thread in browser
	caps.supportsPointerLock = false;  // Disabled for point-and-click gameplay
	caps.supportsFullscreen = true;  // Browser fullscreen API available
	caps.maxTextureSize = 4096;  // Conservative WebGL limit
	caps.defaultWindowWidth = 640;
	caps.defaultWindowHeight = 480;
	return caps;
}
