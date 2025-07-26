#include "UdpSender.h"

int UdpSender::socketFd = -1;
sockaddr_in UdpSender::destAddr;
bool UdpSender::initialized = false;

void UdpSender::setPort(int port)
{
    ensureInitialized();
    destAddr.sin_port = htons(port);
}

void UdpSender::setHost(const std::string& hostname)
{
    ensureInitialized();
    struct hostent* host = gethostbyname(hostname.c_str());
    if (!host)
        PANIC("Failed to resolve hostname");

    destAddr.sin_addr = *(struct in_addr*)host->h_addr;
}

void UdpSender::send(const std::string& type, const std::string& jsonPayload)
{
    ensureInitialized();

    std::string message = type + "|" + jsonPayload;
    sendto(socketFd, message.c_str(), message.size(), 0,
           (sockaddr*)&destAddr, sizeof(destAddr));
}

void UdpSender::ensureInitialized()
{
    if (initialized)
        return;

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
        PANIC("Failed to create UDP socket");

    std::memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(12345); // default port
    inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr);

    initialized = true;
}

void UdpSender::shutdown()
{
    if (socketFd >= 0)
        close(socketFd);

    socketFd = -1;
    initialized = false;
}
