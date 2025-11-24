#pragma once
#include "Application.h"
#include <thread>
#include <atomic>
#include <chrono>

class Game;

class Helbreath : public core::Application {
public:
    Helbreath();
	~Helbreath() override;

    static Helbreath& Instance() {
        if (!s_Instance) {
            throw std::runtime_error("Helbreath instance is not initialized.");
        }
        return *s_Instance;
	}

protected:
    Game* m_pGame = nullptr;

    void InitializeComponents() override;
    void FocusLost() override;
    void FocusGained() override;

private:
    static Helbreath* s_Instance;

private:
    std::thread pulseThread;
    std::atomic<bool> running{ false };

    void PulseLoop();

    void StartPulse() {
        running = true;
        pulseThread = std::thread(&Helbreath::PulseLoop, this);
    }

    void StopPulse() {
        running = false;
        if (pulseThread.joinable()) {
            pulseThread.join();
        }
    }
};

