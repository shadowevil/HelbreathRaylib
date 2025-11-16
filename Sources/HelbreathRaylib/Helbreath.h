#pragma once
#include "Application.h"
#include "XSocket.h"
#include <thread>
#include <atomic>
#include <chrono>

class Game;

class Helbreath : public core::Application {
public:
    Helbreath();
	~Helbreath() override;

protected:
    Game* m_pGame = nullptr;

    void InitializeComponents() override;
    void FocusLost() override;
    void FocusGained() override;

private:
    static Helbreath* s_Instance;
#ifdef _WIN32
    std::unique_ptr<XSocket> m_pLoginServer{ nullptr };
	static void OnLoginServerReceive(const MsgData& msg);
	void OnLoginServerReceiveImpl(const MsgData& msg);

    std::unique_ptr<XSocket> m_pGameServer{ nullptr };
	static void OnGameServerReceive(const MsgData& msg);
	void OnGameServerReceiveImpl(const MsgData& msg);
#endif

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

