#pragma once
#include "raylib_include.h"
#include "Event.h"

namespace core {

	// Forward declaration
	class SceneManager;

	// Base Scene class
	// Scenes represent distinct game states (menu, gameplay, pause, etc.)
	class Scene {
	public:
		Scene(const std::string& name = "Scene")
			: m_name(name), m_initialized(false) {}

		virtual ~Scene() = default;

		// Non-virtual wrapper that handles initialization state
		void Initialize() {
			if (!m_initialized) {
				OnEnter();
				m_initialized = true;
			}
		}

		// Non-virtual wrapper that handles cleanup state
		void Uninitialize() {
			if (m_initialized) {
				OnExit();
				m_initialized = false;
			}
		}

		// Called every frame for logic updates
		virtual void Update(float deltaTime) {}

		// Called every frame for rendering
		virtual void Render() {}

		// Called when an event occurs
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_name; }
		inline bool IsInitialized() const { return m_initialized; }

	protected:
		// Override these in derived classes
		// Called when the scene becomes active
		virtual void OnEnter() {}

		// Called when the scene becomes inactive
		virtual void OnExit() {}

		std::string m_name;
		bool m_initialized;

		// Access to scene manager for requesting transitions
		friend class SceneManager;
	};

	// Scene transition types
	enum class SceneTransition {
		None,        // Instant transition
		Fade,        // Fade out, fade in
		Slide,       // Slide transition
		Custom       // User-defined transition
	};

	// Scene Manager - manages scene lifecycle and transitions
	class SceneManager {
	public:
		SceneManager();
		~SceneManager();

		// Add a scene to the manager (does not activate it)
		template<typename T, typename... Args>
		T* AddScene(const std::string& name, Args&&... args);

		// Switch to a scene by name
		void SwitchTo(const std::string& name, SceneTransition transition = SceneTransition::None);

		// Update current scene and handle transitions
		void Update(float deltaTime);

		// Render current scene and transitions
		void Render();

		// Handle events for current scene
		void OnEvent(Event& event);

		// Get current scene
		Scene* GetCurrentScene() { return m_currentScene; }

		// Check if transitioning
		bool IsTransitioning() const { return m_isTransitioning; }

	private:
		void StartTransition(const std::string& targetScene, SceneTransition transition);
		void UpdateTransition(float deltaTime);
		void CompleteTransition();

	private:
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
		Scene* m_currentScene;
		Scene* m_nextScene;

		bool m_isTransitioning;
		SceneTransition m_transitionType;
		float m_transitionProgress;
		float m_transitionDuration;
	};

	// Template implementation
	template<typename T, typename... Args>
	T* SceneManager::AddScene(const std::string& name, Args&&... args) {
		static_assert(std::is_base_of<Scene, T>::value, "T must derive from Scene");

		auto scene = std::make_unique<T>(std::forward<Args>(args)...);
		T* scenePtr = scene.get();

		m_scenes[name] = std::move(scene);

		// If this is the first scene, make it current
		if (m_currentScene == nullptr) {
			m_currentScene = scenePtr;
			m_currentScene->Initialize();
		}

		return scenePtr;
	}

} // namespace core
