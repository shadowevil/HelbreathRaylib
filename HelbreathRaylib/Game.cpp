#include "Game.h"

Game::Game()
	: Layer("Game") {
}

void Game::Initialize() {
	Layer::Initialize();
	// Initialize game resources here
}

void Game::Uninitialize() {
	// Cleanup game resources here
	Layer::Uninitialize();
}

void Game::Update(float deltaTime) {
	// Update game logic here
}

void Game::Render() {
	// Render game here
}

void Game::OnEvent(core::Event& event) {
	// Handle game events here
}
