#pragma once
#include "Application.h"
#include "XSocket.h"

class Game;

class Helbreath : public core::Application {
public:
    Helbreath();
	~Helbreath() override;

protected:
    Game* m_pGame;

    void InitializeComponents() override;

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
};

