#pragma once
#include "SceneManager.h"
#include "Layer.h"
#include "Helbreath.h"
#include "Sprite.h"

class Game : public core::Layer {
public:
	Game(core::Application& app) : core::Layer(app) {}
	void OnPulse();
	void OnInitialize() override;
	void OnUninitialize() override;
	void OnUpdate() override;
	void OnRender() override;

	CSpriteCollection m_sprites{};
	std::unique_ptr<core::SceneManager> m_pSceneManager{ nullptr };
};

