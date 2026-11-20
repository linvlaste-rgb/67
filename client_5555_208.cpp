#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

void receive_messages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int count = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (count <= 0) break;
        buffer[count] = '\0';
        std::cout << "\n" << buffer << "\n";
    }
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5555);
    inet_pton(AF_INET, "192.168.88.208", &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Не удалось подключиться к серверу.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Подключено! Пишите:\n";
    std::thread receiver(receive_messages, sock);

    std::string msg;
    while (std::getline(std::cin, msg)) {
        if (send(sock, msg.c_str(), (int)msg.size(), 0) == SOCKET_ERROR) break;
    }

    shutdown(sock, SD_BOTH);
    receiver.join();
    closesocket(sock);
    WSACleanup();
    return 0;
}
