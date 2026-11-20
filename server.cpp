#include <iostream>
#include <cstring>
#include <clocale>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(0, "");
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) { WSACleanup(); return 1; }
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR ||
        listen(server, 1) == SOCKET_ERROR) {
        std::cout << "Server error\n";
        closesocket(server);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for client...\n";
    SOCKET client = accept(server, NULL, NULL);
    if (client == INVALID_SOCKET) {
        closesocket(server);
        WSACleanup();
        return 1;
    }
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) break;
        std::cout << "Received: " << buffer << "\n";
        if (send(client, buffer, received, 0) == SOCKET_ERROR) break;
    }
    std::cout << "Client disconnected\n";
    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
