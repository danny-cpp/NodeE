#include "InstructionToken.h"

ServerClient::InstructionToken::InstructionToken(const char *json_string) {

    std::string err;
    json11::Json test_obj = json11::Json::parse(json_string, err);

    sender_id = test_obj["sender_id"].int_value();
    task_id  = test_obj["task_id"].int_value();
    interface_type = test_obj["interface_type"].string_value();
    api_call = test_obj["api_call"].string_value();
    payload_total_fragments = test_obj["payload_total_fragments"].int_value();
    payload_frag_number = test_obj["payload_frag_number"].int_value();
    payload_size = test_obj["payload_size"].int_value();
    payload_content = test_obj["payload_content"].string_value();
}
