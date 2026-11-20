#include <iostream>
#include <string>
#include <cstring>
#include <clocale>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(0, "");
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { WSACleanup(); return 1; }
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    if (connect(sock, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cout << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::string message;
    char buffer[1024];
    while (true) {
        std::cout << "Message: ";
        if (!std::getline(std::cin, message) || message == "exit") break;
        if (message.empty()) continue;
        if (send(sock, message.c_str(), (int)message.size(), 0) == SOCKET_ERROR) break;
        memset(buffer, 0, sizeof(buffer));
        int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;
        std::cout << "Reply: " << buffer << "\n";
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
