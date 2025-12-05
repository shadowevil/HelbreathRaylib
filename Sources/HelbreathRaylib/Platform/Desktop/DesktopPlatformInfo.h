#pragma once
#include "../IPlatformInfo.h"

/**
 * Desktop platform information implementation.
 * Provides metadata about desktop platform capabilities.
 */
class DesktopPlatformInfo : public IPlatformInfo {
public:
	DesktopPlatformInfo() = default;
	~DesktopPlatformInfo() override = default;

	PlatformType getType() const override;
	const char* getName() const override;
	PlatformCapabilities getCapabilities() const override;
};
