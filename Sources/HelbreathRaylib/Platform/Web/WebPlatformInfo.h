#pragma once
#include "../IPlatformInfo.h"

/**
 * Web platform information implementation.
 * Provides metadata about web platform capabilities.
 */
class WebPlatformInfo : public IPlatformInfo {
public:
	WebPlatformInfo() = default;
	~WebPlatformInfo() override = default;

	PlatformType getType() const override;
	const char* getName() const override;
	PlatformCapabilities getCapabilities() const override;
};
