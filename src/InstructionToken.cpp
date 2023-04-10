#include "InstructionToken.h"

#include <utility>

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
    payload_hexstring = test_obj["payload_content"].string_value();

    payload_content_bytesize = payload_hexstring.size() / 2;
    payload_content = new uint8_t[payload_content_bytesize];
    for(int i = 0; i < payload_hexstring.size(); i+=2) {
        std::string s = payload_hexstring.substr(i, 2);
        uint8_t b;
        std::stringstream ss;
        ss << std::hex << s;
        ss >> b;
        payload_content[i/2] = b;
    }        
}

std::string ServerClient::InstructionToken::dump() {

    std::stringstream stringbuilder;

    stringbuilder   << "{"
                    << "\"sender_id\""                  << ":"              << this->sender_id                  << ","
                    << "\"task_id\""                    << ":"              << this->task_id                    << ","
                    << "\"interface_type\""             << ":"     << "\""  << this->interface_type << "\""     << ","
                    << "\"api_call\""                   << ":"     << "\""  << this->api_call       << "\""     << ","
                    << "\"payload_total_fragments\""    << ":"              << this->payload_total_fragments    << ","
                    << "\"payload_frag_number\""        << ":"              << this->payload_frag_number        << ","
                    << "\"payload_size\""               << ":"              << this->payload_size               << ","
                    << "\"payload_content\""            << ":" << "\""      << this->payload_hexstring << "\""
                    << "}\n";

    return stringbuilder.str();
}

ServerClient::InstructionToken::InstructionToken(int senderId, int taskId, std::string  interfaceType,
                                                 std::string apiCall, int payloadTotalFragments,
                                                 int payloadFragNumber, int payloadSize,
                                                 uint8_t *payloadContent, int payloadContentBytesize) : 
                                                                                      sender_id(senderId),
                                                                                      task_id(taskId),
                                                                                      interface_type(std::move(interfaceType)),
                                                                                      api_call(std::move(apiCall)),
                                                                                      payload_total_fragments(
                                                                                              payloadTotalFragments),
                                                                                      payload_frag_number(
                                                                                              payloadFragNumber),
                                                                                      payload_size(payloadSize),
                                                                                      payload_content(std::move(payloadContent)),
                                                                                      payload_content_bytesize(payloadContentBytesize))
{
    // for(int i = 0; i < payload_content_bytesize; i++) {
    //     std::stringstream ss;

    // }
    

}
