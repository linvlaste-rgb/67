#include <iostream>
#include <string>
#include <clocale>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(0, "");
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) { WSACleanup(); return 1; }
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.88.207", &addr.sin_addr);
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "Connection failed: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected! Type exit to stop.\n";
    std::string message;
    char buffer[1024];
    while (true) {
        std::cout << "You: ";
        if (!std::getline(std::cin, message)) break;
        if (message.empty()) message = " ";
        if (send(sock, message.c_str(), (int)message.size(), 0) == SOCKET_ERROR) break;
        if (message == "exit") break;
        int count = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (count <= 0) break;
        buffer[count] = '\0';
        std::cout << "PC 207: " << buffer << "\n";
        if (std::string(buffer) == "exit") break;
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
