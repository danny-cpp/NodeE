#include "ClientNetworking.h"

void ServerClient::ClientNetworking::producing(int &sock, int port_num, const std::string &IP_string,
                                                 std::deque<InstructionToken> *incoming,
                                                 std::mutex &lock_incoming, std::condition_variable &not_empty_trigger,
                                                 struct sockaddr_in &socket_address) {


    if (inet_pton(AF_INET, IP_string.c_str(), &socket_address.sin_addr) <= 0) {
        perror("Failed to use IP address");
        exit(-1);
    }

    if (connect(sock, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
        perror("Failed to connect");
        exit(-1);
    }

    char* buffer;
    while (true) {
        buffer = new char[buffer_length + 1];
        int byte_length = read(sock, buffer, buffer_length - 1);
        buffer[byte_length] = 0x00;

        if (buffer[0] == '\n') {
            continue;
        }

        lock_incoming.lock();
        incoming->emplace_back(buffer);
        lock_incoming.unlock();
        not_empty_trigger.notify_one();

        delete[] buffer;
    }

    close(sock);
}

void ServerClient::ClientNetworking::sending(int &sock, int port_num, const std::string &IP_string,
                                             std::deque<InstructionToken> *outgoing, std::mutex &lock_outgoing,
                                             std::condition_variable &not_empty_trigger, sockaddr_in &socket_address) {

    while(true) {

        {
            std::unique_lock<std::mutex> lock{lock_outgoing};
            while (outgoing->empty()) {
                not_empty_trigger.wait(lock, [&] {
                    return !outgoing->empty();
                });
            }
            InstructionToken acquired = outgoing->front();
            outgoing->pop_front();
            std::string temp = acquired.dump();
            write(sock, temp.c_str(), temp.size());
        }
    }

}
