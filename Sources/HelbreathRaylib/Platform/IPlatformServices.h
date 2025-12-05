#pragma once
#include <memory>

// Forward declarations
class ICursorManager;
class IInputTranslator;
class ITimerService;
class IPlatformInfo;

enum class PlatformType {
	Desktop,
	Web,
	Unknown
};

/**
 * Main platform abstraction interface.
 * Provides access to all platform-specific services.
 * Game logic depends on this interface, not concrete implementations.
 */
class IPlatformServices {
public:
	virtual ~IPlatformServices() = default;

	// Get platform-specific service implementations
	virtual ICursorManager& getCursorManager() = 0;
	virtual IInputTranslator& getInputTranslator() = 0;
	virtual ITimerService& getTimerService() = 0;
	virtual IPlatformInfo& getPlatformInfo() = 0;

	// Only call if absolutely necessary (rare edge cases)
	virtual PlatformType getPlatformType() const = 0;
};
