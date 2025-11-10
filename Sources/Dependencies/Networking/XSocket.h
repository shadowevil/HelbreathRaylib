#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

#pragma comment(lib, "Ws2_32.lib")

using MsgData = std::vector<uint8_t>;

class XSocket
{
public:
    using RecvCallback = std::function<void(const MsgData&)>;

    XSocket() : sock(INVALID_SOCKET), running(false) {}
    ~XSocket() { Disconnect(); }

    bool Connect(const std::string& host, uint16_t port)
    {
        if (running) return false;

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) return false;

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
            return false;
        }

        running = true;
        worker = std::thread([this]() { ThreadLoop(); });
        return true;
    }

    void Disconnect()
    {
        if (!running) return;
        running = false;
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        if (worker.joinable()) worker.join();
        sock = INVALID_SOCKET;
    }

    void SetReceiveCallback(RecvCallback cb)
    {
        onRecv = cb;
    }

    bool Send(const uint8_t* data, size_t size, uint8_t key)
    {
        if (!running || sock == INVALID_SOCKET) return false;
        if (size > 65535) return false; // arbitrary limit

        std::vector<uint8_t> buf;
        buf.reserve(size + 3);

        buf.push_back(key);
        uint16_t total = (uint16_t)(size + 3);
        buf.push_back((uint8_t)(total & 0xFF));
        buf.push_back((uint8_t)((total >> 8) & 0xFF));

        for (size_t i = 0; i < size; i++)
            buf.push_back(data[i]);

        if (key != 0)
        {
            for (size_t i = 0; i < size; i++)
            {
                buf[3 + i] += (uint8_t)(i ^ key);
                buf[3 + i] = buf[3 + i] ^ (uint8_t)(key ^ (size - i));
            }
        }

        std::lock_guard<std::mutex> lock(sendMutex);
        const uint8_t* p = buf.data();
        size_t sent = 0;
        while (sent < buf.size())
        {
            int r = send(sock, (const char*)p + sent, (int)(buf.size() - sent), 0);
            if (r <= 0) return false;
            sent += r;
        }
        return true;
    }

    bool AdoptSocket(SOCKET s)
    {
        if (running || sock != INVALID_SOCKET) return false;
        sock = s;
        running = true;
        worker = std::thread([this]() { ThreadLoop(); });
        return true;
    }

private:
    void ThreadLoop()
    {
        std::vector<uint8_t> header(3);
        while (running)
        {
            // read header
            if (!RecvAll(header.data(), 3)) break;

            uint8_t key = header[0];
            uint16_t total = (uint16_t)header[1] | ((uint16_t)header[2] << 8);
            uint16_t bodySize = total - 3;

            std::vector<uint8_t> body(bodySize);
            if (!RecvAll(body.data(), bodySize)) break;

            if (key != 0)
            {
                for (size_t i = 0; i < bodySize; i++)
                {
                    body[i] = body[i] ^ (uint8_t)(key ^ (bodySize - i));
                    body[i] -= (uint8_t)(i ^ key);
                }
            }

            if (onRecv) onRecv(body);
        }
        running = false;
    }

    bool RecvAll(void* buf, size_t len)
    {
        uint8_t* p = (uint8_t*)buf;
        size_t recvd = 0;
        while (recvd < len && running)
        {
            int r = recv(sock, (char*)p + recvd, (int)(len - recvd), 0);
            if (r <= 0) return false;
            recvd += r;
        }
        return recvd == len;
    }

    SOCKET sock;
    std::atomic<bool> running;
    std::thread worker;
    std::mutex sendMutex;
    RecvCallback onRecv;
};
