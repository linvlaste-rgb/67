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
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(5555);
    address.sin_addr.s_addr = INADDR_ANY;
    if (bind(server, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR || listen(server, 1) == SOCKET_ERROR) {
        std::cout << "Server error: " << WSAGetLastError() << "\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for PC 209...\n";
    SOCKET client = accept(server, NULL, NULL);
    if (client == INVALID_SOCKET) { closesocket(server); WSACleanup(); return 1; }
    std::cout << "Connected. Type exit to stop.\n";
    char buffer[1024];
    std::string message;
    while (true) {
        int count = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (count <= 0) break;
        buffer[count] = '\0';
        std::cout << "PC 209: " << buffer << "\n";
        if (std::string(buffer) == "exit") break;
        std::cout << "You: ";
        if (!std::getline(std::cin, message)) break;
        if (message.empty()) message = " ";
        if (send(client, message.c_str(), (int)message.size(), 0) == SOCKET_ERROR) break;
        if (message == "exit") break;
    }
    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
