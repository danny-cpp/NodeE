#include "ClientNetworking.h"

ServerClient::ClientNetworking::ClientNetworking(int port_num, const std::string &IP_string,
                                                 std::deque<InstructionToken> *incoming,
                                                 std::mutex &lock_incoming,
                                                 std::condition_variable &not_empty_trigger) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Fail to create socket");
        exit(-1);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port_num);

    if (inet_pton(AF_INET, IP_string.c_str(), &socket_address.sin_addr) <= 0) {
        perror("Failed to use IP address");
        exit(-1);
    }

    if (connect(sock, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        perror("Failed to connect");
        exit(-1);
    }

    while (true) {
        buffer = new char[buffer_length + 1];
        int byte_length = read(sock, buffer, buffer_length - 1);
        buffer[byte_length] = 0x00;

        lock_incoming.lock();
        incoming->emplace_back(buffer);
        lock_incoming.unlock();
        not_empty_trigger.notify_one();

        delete[] buffer;
    }

    close(sock);
}
