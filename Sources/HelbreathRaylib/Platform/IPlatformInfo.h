#pragma once
#include "IPlatformServices.h"

/**
 * Platform capabilities and metadata.
 * Provides information about what the platform supports.
 */
struct PlatformCapabilities {
	bool supportsThreading;      // Can use std::thread
	bool supportsPointerLock;    // Can lock cursor to window
	bool supportsFullscreen;     // Can toggle fullscreen
	int maxTextureSize;          // Maximum texture dimension
	int defaultWindowWidth;      // Recommended window width
	int defaultWindowHeight;     // Recommended window height
};

/**
 * Platform information interface.
 * Provides metadata about the current platform.
 */
class IPlatformInfo {
public:
	virtual ~IPlatformInfo() = default;

	// Get platform type
	virtual PlatformType getType() const = 0;

	// Get platform name (e.g., "Desktop", "Web")
	virtual const char* getName() const = 0;

	// Get platform capabilities
	virtual PlatformCapabilities getCapabilities() const = 0;
};
