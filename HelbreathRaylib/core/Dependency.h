#pragma once
#include "raylib_include.h"

namespace core {

	// Base class for all managed dependencies/resources
	// Provides a common interface for loading, unloading, and tracking usage
	class Dependency {
	public:
		Dependency() : m_loaded(false), m_lastAccessTime(0.0f) {}
		virtual ~Dependency() = default;

		// Load the dependency (must be implemented by derived classes)
		virtual void Load() = 0;

		// Unload the dependency (must be implemented by derived classes)
		virtual void Unload() = 0;

		// Check if the dependency is currently loaded
		bool IsLoaded() const { return m_loaded; }

		// Update the last access time (for caching)
		void UpdateAccessTime(float currentTime) { m_lastAccessTime = currentTime; }

		// Get the last access time
		float GetLastAccessTime() const { return m_lastAccessTime; }

	protected:
		bool m_loaded;
		float m_lastAccessTime;
	};

} // namespace core
