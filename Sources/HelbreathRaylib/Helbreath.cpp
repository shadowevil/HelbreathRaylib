#include <memory>
#include "Helbreath.h"
#include "Game.h"

Helbreath* Helbreath::s_Instance = nullptr;

Helbreath::Helbreath()
	: Application(core::Window::Config{
		"Helbreath Raylib",
		800, 600,
		FLAG_VSYNC_HINT,
		60
		}) {
#ifdef _WIN32
	// init winsock
	WSADATA w;
	if (WSAStartup(MAKEWORD(2, 2), &w) != 0)
		throw std::runtime_error("WSAStartup failed");
#endif
}

Helbreath::~Helbreath() {
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