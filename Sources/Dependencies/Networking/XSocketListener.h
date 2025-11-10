#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
#include <thread>
#include <functional>
#include <string>
#include <memory>

/*
  XSocketListener
    Accepts incoming TCP connections on a specified address and port.
    Runs a dedicated thread waiting on accept().
    On each successful accept, invokes a callback with the raw SOCKET.
    Intended to hand accepted sockets to XSocket for actual send/recv work.
    Does not process data, only establishes connections.
*/
class XSocketListener
{
public:
    using AcceptCallback = std::function<void(SOCKET)>;

    XSocketListener() : sock(INVALID_SOCKET), running(false) {}
    ~XSocketListener() { Stop(); }

    bool Start(const std::string& host, uint16_t port, AcceptCallback cb)
    {
        if (running) return false;

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) return false;
        if (listen(sock, SOMAXCONN) == SOCKET_ERROR) return false;

        onAccept = cb;
        running = true;
        worker = std::thread([this]() { Loop(); });
        return true;
    }

    void Stop()
    {
        if (!running) return;
        running = false;
        closesocket(sock);
        if (worker.joinable()) worker.join();
        sock = INVALID_SOCKET;
    }

private:
    void Loop()
    {
        while (running)
        {
            SOCKET s = accept(sock, nullptr, nullptr);
            if (s == INVALID_SOCKET) continue;
            if (onAccept) onAccept(s);
        }
    }

    SOCKET sock;
    std::atomic<bool> running;
    std::thread worker;
    AcceptCallback onAccept;
};