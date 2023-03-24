#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <deque>
#include <condition_variable>


#include "InstructionToken.h"


namespace ServerClient {
    class ClientNetworking {
    public:
        ClientNetworking(int port_num, const std::string &IP_string, std::deque<InstructionToken> *incoming,
                         std::mutex &lock_incoming, std::condition_variable &not_empty_trigger);
    private:
        struct sockaddr_in socket_address;
        int sock;
        char* buffer;
        int buffer_length = 4096;
    };
}
