#include "Scene.h"

namespace core {

	SceneManager::SceneManager()
		: m_currentScene(nullptr), m_nextScene(nullptr),
		  m_isTransitioning(false), m_transitionType(SceneTransition::None),
		  m_transitionProgress(0.0f), m_transitionDuration(0.5f) {
	}

	SceneManager::~SceneManager() {
		// Uninitialize current scene
		if (m_currentScene && m_currentScene->IsInitialized()) {
			m_currentScene->Uninitialize();
		}

		// Clear all scenes
		m_scenes.clear();
	}

	void SceneManager::SwitchTo(const std::string& name, SceneTransition transition) {
		auto it = m_scenes.find(name);
		if (it == m_scenes.end()) {
			// Scene not found
			return;
		}

		Scene* targetScene = it->second.get();
		if (targetScene == m_currentScene) {
			// Already on this scene
			return;
		}

		if (transition == SceneTransition::None) {
			// Instant transition
			if (m_currentScene) {
				m_currentScene->Uninitialize();
			}

			m_currentScene = targetScene;
			m_currentScene->Initialize();
		}
		else {
			// Start transition
			StartTransition(name, transition);
		}
	}

	void SceneManager::Update(float deltaTime) {
		if (m_isTransitioning) {
			UpdateTransition(deltaTime);
		}
		else if (m_currentScene) {
			m_currentScene->Update(deltaTime);
		}
	}

	void SceneManager::Render() {
		if (m_currentScene) {
			m_currentScene->Render();
		}

		// Render transition overlay
		if (m_isTransitioning) {
			switch (m_transitionType) {
			case SceneTransition::Fade: {
				// Fade out/in effect
				float alpha = (m_transitionProgress < 0.5f)
					? m_transitionProgress * 2.0f
					: (1.0f - m_transitionProgress) * 2.0f;

				DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
					ColorAlpha(BLACK, alpha));
				break;
			}
			case SceneTransition::Slide:
				// TODO: Implement slide transition
				break;
			case SceneTransition::Custom:
				// User-defined transitions can be handled in derived classes
				break;
			default:
				break;
			}
		}
	}

	void SceneManager::OnEvent(Event& event) {
		if (!m_isTransitioning && m_currentScene) {
			m_currentScene->OnEvent(event);
		}
	}

	void SceneManager::StartTransition(const std::string& targetScene, SceneTransition transition) {
		auto it = m_scenes.find(targetScene);
		if (it == m_scenes.end()) {
			return;
		}

		m_nextScene = it->second.get();
		m_isTransitioning = true;
		m_transitionType = transition;
		m_transitionProgress = 0.0f;

		// Set duration based on transition type
		switch (transition) {
		case SceneTransition::Fade:
			m_transitionDuration = 0.5f;
			break;
		case SceneTransition::Slide:
			m_transitionDuration = 0.3f;
			break;
		default:
			m_transitionDuration = 0.5f;
			break;
		}
	}

	void SceneManager::UpdateTransition(float deltaTime) {
		m_transitionProgress += deltaTime / m_transitionDuration;

		// Switch scenes at halfway point
		if (m_transitionProgress >= 0.5f && m_currentScene != nullptr) {
			m_currentScene->Uninitialize();
			m_currentScene = m_nextScene;
			m_currentScene->Initialize();
			m_nextScene = nullptr;
		}

		// Complete transition
		if (m_transitionProgress >= 1.0f) {
			CompleteTransition();
		}
	}

	void SceneManager::CompleteTransition() {
		m_isTransitioning = false;
		m_transitionProgress = 0.0f;
		m_nextScene = nullptr;
	}

} // namespace core
