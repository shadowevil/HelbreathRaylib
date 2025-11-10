#pragma once
#include "raylib_include.h"
#include "core/Application.h"

// Main Helbreath Application
class Helbreath : public core::Application {
public:
	Helbreath();
	~Helbreath() override = default;

protected:
	void OnInitialize() override;
	void OnShutdown() override;
};
