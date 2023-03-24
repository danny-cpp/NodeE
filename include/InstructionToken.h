#pragma once

#include <string>
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
        std::string payload_content;

        InstructionToken(const char* json_string);
    };
}
