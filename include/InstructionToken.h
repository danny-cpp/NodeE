#pragma once

#include <string>
#include <sstream>
#include <cstdio>
#include "json11.hpp"


namespace ServerClient {
    class InstructionToken {
    public:

        int sender_id;
        int task_id;
        std::string interface_type;
        std::string api_call;
        int payload_total_fragments;
        int payload_frag_number;
        int payload_size;
        std::string payload_hexstring;
        uint8_t *payload_content;

        InstructionToken(const char* json_string);

        InstructionToken(int senderId, int taskId, std::string interfaceType, std::string apiCall,
                         int payloadTotalFragments, int payloadFragNumber, int payloadSize,
                         uint8_t *payloadContent, int payloadContentBytesize);

        std::string dump();

        ~InstructionToken() {
            delete[] payload_content;
        }

    private:
        int payload_content_bytesize;
    };
}
