#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <csignal>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::atomic<bool> connected(true);

bool sendAll(int fd, const std::string& text) {
    size_t sent = 0;
    while (sent < text.size()) {
        ssize_t n = send(fd, text.data() + sent, text.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

void receiveMessages(int fd) {
    char buffer[1024];
    std::string pending;
    while (connected) {
        ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        pending.append(buffer, n);
        size_t pos;
        while ((pos = pending.find('\n')) != std::string::npos) {
            std::string message = pending.substr(0, pos);
            pending.erase(0, pos + 1);
            if (message == "/exit") {
                connected = false;
                std::cout << "\nOther PC disconnected.\n";
                shutdown(fd, SHUT_RDWR);
                return;
            }
            std::cout << "\nOther PC: " << message << "\nYou: " << std::flush;
        }
    }
    connected = false;
    std::cout << "\nConnection closed.\n";
    shutdown(fd, SHUT_RDWR);
}

void chat(int fd) {
    std::thread receiver(receiveMessages, fd);
    std::string message;
    std::cout << "Connected! Type /exit to quit.\n";
    while (connected && std::getline(std::cin, message)) {
        if (!sendAll(fd, message + "\n")) break;
        if (message == "/exit") break;
        std::cout << "You: " << std::flush;
    }
    connected = false;
    shutdown(fd, SHUT_RDWR);
    receiver.join();
}

int main() {
    std::signal(SIGPIPE, SIG_IGN);
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(5555);
    if (inet_pton(AF_INET, "192.168.88.226", &address.sin_addr) != 1) {
        std::cerr << "Invalid server IP\n"; close(server); return 1;
    }
    if (bind(server, (sockaddr*)&address, sizeof(address)) < 0 || listen(server, 1) < 0) {
        perror("server"); close(server); return 1;
    }
    std::cout << "Server 192.168.88.226:5555 waiting for client...\n";
    int client = accept(server, nullptr, nullptr);
    if (client < 0) { perror("accept"); close(server); return 1; }
    chat(client);
    close(client);
    close(server);
    return 0;
}
