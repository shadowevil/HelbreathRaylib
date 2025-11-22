#include <memory>
#include "Helbreath.h"
#include "Game.h"

Helbreath* Helbreath::s_Instance = nullptr;

Helbreath::Helbreath()
	: Application(core::Window::Config{
		"Helbreath Raylib",
		800, 600,
		FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT,
		0
		}) {
#ifdef _WIN32
	// init winsock
	WSADATA w;
	if (WSAStartup(MAKEWORD(2, 2), &w) != 0)
		throw std::runtime_error("WSAStartup failed");
#endif
}

Helbreath::~Helbreath() {
	StopPulse();
#ifdef _WIN32
	WSACleanup();
#endif
}

void Helbreath::InitializeComponents() {
	s_Instance = this;

#ifdef _WIN32
	m_pLoginServer = std::make_unique<XSocket>();
	m_pGameServer = std::make_unique<XSocket>();
	m_pLoginServer->SetReceiveCallback(OnLoginServerReceive);
	m_pGameServer->SetReceiveCallback(OnGameServerReceive);
#endif

	m_pGame = PushLayer<Game>();
	StartPulse();
}

void Helbreath::FocusLost()
{
	rlx::UnlockCursor();
}

void Helbreath::FocusGained()
{
	rlx::LockCursor(constant::BASE_WIDTH, constant::BASE_HEIGHT);
}

#ifdef _WIN32
void Helbreath::OnLoginServerReceive(const MsgData& msg)
{
	if (!s_Instance)
		return;

	if (!s_Instance->m_pLoginServer)
		return;

	s_Instance->OnLoginServerReceiveImpl(msg);
}

void Helbreath::OnGameServerReceive(const MsgData& msg)
{
	if (!s_Instance)
		return;

	if (!s_Instance->m_pLoginServer)
		return;

	s_Instance->OnGameServerReceiveImpl(msg);
}

void Helbreath::OnLoginServerReceiveImpl(const MsgData& msg)
{

}

void Helbreath::OnGameServerReceiveImpl(const MsgData& msg)
{

}
#endif

void Helbreath::PulseLoop() {
	while (running) {
		// Perform periodic tasks here
		m_pGame->OnPulse();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}