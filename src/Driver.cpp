#include "iostream"
#include "string"
#include "thread"
#include "condition_variable"
#include "chrono"

#include "QPP.h"
#include "ClientNetworking.h"
#include "InstructionToken.h"


int main() {

    std::cout << "                 _      _____ \n"
                 " _ __   ___   __| | ___| ____|\n"
                 "| '_ \\ / _ \\ / _` |/ _ \\  _|  \n"
                 "| | | | (_) | (_| |  __/ |___ \n"
                 "|_| |_|\\___/ \\__,_|\\___|_____|" << std::endl;


    std::deque<ServerClient::InstructionToken> incoming_buffer;
    std::mutex shared_pool_lock;
    std::condition_variable not_empty_trigger;

    std::thread producer([&] {
        ServerClient::ClientNetworking connector(64998, "127.0.0.1", &incoming_buffer, shared_pool_lock, not_empty_trigger);
    });

    // const std::string json_string = R"({"api_call":"RECEIVED_ID","task_id":"2","interface_type":"T1","sender_id":1})";
    // std::string err;
    // json11::Json test_obj = json11::Json::parse(json_string, err);

    // int tmp = test_obj["sender_id"].int_value();
    // // int a = atoi(tmp.c_str());
    // std::cout << tmp << std::endl;
    // ServerClient::InstructionToken tk(json_string.c_str());


    // int text_size = 12;
    uint8_t* seed = new uint8_t[Xceed::Constants::block_size];
    for (int i = 0; i < Xceed::Constants::block_size; i++) {
         seed[i] = (uint8_t)random();
    }

    std::thread consumer([&] {
        while (true) {

            ServerClient::InstructionToken *acquired = nullptr;
            {
                std::unique_lock<std::mutex> lock{shared_pool_lock};
                while (incoming_buffer.empty()) {
                    not_empty_trigger.wait(lock, [&] {
                        return !incoming_buffer.empty();
                    });
                }
                acquired = new ServerClient::InstructionToken(incoming_buffer.front());
                incoming_buffer.pop_front();
            }

            uint8_t* reversed;
            if (acquired->api_call == "ENCRYPT") {
                std::cout << "Encrypt is called!" << std::endl;
                uint8_t* cipher;
                {
                    Xceed::QPP qpp(seed);
                    qpp.setPlainText((uint8_t*)acquired->payload_content.c_str(), acquired->payload_size);
                    qpp.setSeed(seed, acquired->payload_size);
                    cipher = qpp.encrypt();
                }

                {
                    Xceed::QPP qpp2(seed);
                    // uint8_t ciphered[] = {0xa8, 0xc7, 0xde, 0xe9, 0x66, 0x64, 0x09, 0xfb, 0xeb, 0x41, 0x80, 0xc5};
                    qpp2.setCipherText(cipher, acquired->payload_size);
                    reversed = qpp2.decrypt();
                }

            }
            else {
                std::cout << "Message is acquired but API call is undefined" << std::endl;
            }

            std::cout << "Recovered text" << (char*)reversed << std::endl;

            delete acquired;
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
    });

    //
    //
    // uint8_t temp[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0b00100000, 0b01010111, 0b01101111, 0b01110010, 0b01101100, 0b01100100};
    // // Expected cipher a8 c7 de e9   66 64 09 fb   eb 41 80 c5
    //
    // uint8_t* cipher;
    // {
    //     Xceed::QPP qpp(seed);
    //     qpp.setPlainText(temp, text_size);
    //     qpp.setSeed(seed, text_size);
    //     cipher = qpp.encrypt();
    // }
    //
    // uint8_t* reversed;
    // {
    //     Xceed::QPP qpp2(seed);
    //     // uint8_t ciphered[] = {0xa8, 0xc7, 0xde, 0xe9, 0x66, 0x64, 0x09, 0xfb, 0xeb, 0x41, 0x80, 0xc5};
    //     qpp2.setCipherText(cipher, text_size);
    //     reversed = qpp2.decrypt();
    // }
    //
    // delete[] seed;
    // delete[] cipher;
    // delete[] reversed;

    producer.join();
    consumer.join();

    return 0;
}
