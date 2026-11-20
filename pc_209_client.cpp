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
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(5555);
    inet_pton(AF_INET, "192.168.88.208", &address.sin_addr);
    if (connect(sock, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cout << "Connection error: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected. Type exit to stop.\n";
    char buffer[1024];
    std::string message;
    while (true) {
        std::cout << "You: ";
        if (!std::getline(std::cin, message)) break;
        if (message.empty()) message = " ";
        if (send(sock, message.c_str(), (int)message.size(), 0) == SOCKET_ERROR) break;
        if (message == "exit") break;
        int count = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (count <= 0) break;
        buffer[count] = '\0';
        std::cout << "PC 208: " << buffer << "\n";
        if (std::string(buffer) == "exit") break;
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
