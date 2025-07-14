#pragma once
#include <string>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

class UdpSender
{
public:
    // Call once before any send
    static void setPort(int port);

    // Optional: set custom host (default = 127.0.0.1)
    static void setHost(const std::string& hostname);

    // Send message with type prefix, e.g. "level|{...}"
    static void send(const std::string& type, const std::string& jsonPayload);

    // Clean up
    static void shutdown();

private:
    static int socketFd;
    static struct sockaddr_in destAddr;
    static bool initialized;
    static void ensureInitialized();
};
