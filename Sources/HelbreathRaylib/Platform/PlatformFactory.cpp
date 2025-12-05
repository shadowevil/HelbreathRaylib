#include "PlatformFactory.h"

#ifdef __EMSCRIPTEN__
	#include "Web/WebPlatform.h"
#else
	#include "Desktop/DesktopPlatform.h"
#endif

std::unique_ptr<IPlatformServices> PlatformFactory::create() {
#ifdef __EMSCRIPTEN__
	return std::make_unique<WebPlatform>();
#else
	return std::make_unique<DesktopPlatform>();
#endif
}
