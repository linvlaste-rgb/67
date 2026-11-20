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
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) { WSACleanup(); return 1; }
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.88.207", &addr.sin_addr);
    if (bind(server, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ||
        listen(server, 1) == SOCKET_ERROR) {
        std::cout << "Server error: " << WSAGetLastError() << "\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for PC 208...\n";
    SOCKET client = accept(server, NULL, NULL);
    if (client == INVALID_SOCKET) { closesocket(server); WSACleanup(); return 1; }
    std::cout << "Connected! Type exit to stop.\n";
    char buffer[1024];
    std::string reply;
    while (true) {
        int count = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (count <= 0) break;
        buffer[count] = '\0';
        std::cout << "PC 208: " << buffer << "\n";
        if (std::string(buffer) == "exit") break;
        std::cout << "You: ";
        if (!std::getline(std::cin, reply)) break;
        if (reply.empty()) reply = " ";
        if (send(client, reply.c_str(), (int)reply.size(), 0) == SOCKET_ERROR) break;
        if (reply == "exit") break;
    }
    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
