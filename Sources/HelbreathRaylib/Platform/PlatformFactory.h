#pragma once
#include "IPlatformServices.h"
#include <memory>

/**
 * Factory for creating platform-specific implementations.
 * Uses compile-time detection to instantiate the correct platform.
 */
class PlatformFactory {
public:
	// Create the appropriate platform based on compile-time defines
	// Returns unique_ptr to IPlatformServices
	static std::unique_ptr<IPlatformServices> create();

private:
	PlatformFactory() = default;
};
