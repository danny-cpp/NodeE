#include <iostream>
#include <string>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <cstdio>
#include <cstring>

#include "QPP.h"
#include "ClientNetworking.h"
#include "InstructionToken.h"
#include "ThreadSync.h"


static const int core_complex_port = 64998;
static const int nodeE_ID = 3003;


int main() {

    std::cout << "                 _      _____ \n"
                 " _ __   ___   __| | ___| ____|\n"
                 "| '_ \\ / _ \\ / _` |/ _ \\  _|  \n"
                 "| | | | (_) | (_| |  __/ |___ \n"
                 "|_| |_|\\___/ \\__,_|\\___|_____|" << std::endl;


    /**
     * Buffers and sync techniques
     */
    std::deque<ServerClient::InstructionToken> incoming_buffer;
    std::mutex incoming_lock;
    std::condition_variable incoming_not_empty;

    std::deque<ServerClient::InstructionToken> outgoing_buffer;
    std::mutex outgoing_lock;
    std::condition_variable out_going_not_empty;


    /**
     * Networking objects
     */
    int sock;
    struct sockaddr_in socket_address;


    /**
     * Initialization of connection
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Fail to create socket");
        exit(-1);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(core_complex_port);


    /**
     * Listening, parsing incoming data on this thread
     */
    std::thread incoming_thread([&] {
        ServerClient::ClientNetworking::producing(sock, core_complex_port, "127.0.0.1", &incoming_buffer,
                                                 incoming_lock, incoming_not_empty, socket_address);
    });


    /**
     * Sending processed data on this thread
     */
    std::thread outgoing_thread([&] {
       ServerClient::ClientNetworking::sending(sock, core_complex_port, "127.0.0.1", &outgoing_buffer,
                                               outgoing_lock, out_going_not_empty, socket_address);
    });


    /**
     * Data pointers
     */
    Xceed::QPP* qpp;
    uint8_t* seed = new uint8_t[Xceed::Constants::block_size];
//    uint8_t* cipher;
//    uint8_t* plain_text;


    /**
     * Processing tokens on this thread
     */
    using namespace ServerClient;
    using namespace Xceed::ThreadSync;
    std::thread consumer([&] {
        while (true) {

            /**
             * Polling for token
             */
            std::unique_ptr<InstructionToken> acquired(safeAcquire(&incoming_buffer, incoming_lock, incoming_not_empty));


            /**
             * Action menu switch case, as per API standard
             */
            if (acquired->api_call == "REQUEST_HANDSHAKE") {
                std::cout << "Handshaking is initiated by the Core Complex" << std::endl;

                std::unique_ptr<InstructionToken> send_ID_token(new InstructionToken(0, 0, "T1", "SEND_ID", 1,
                                                                                     0, 1, std::to_string(nodeE_ID)));

                {
                    std::unique_lock<std::mutex> lock{outgoing_lock};
                    outgoing_buffer.push_back(*send_ID_token);
                    out_going_not_empty.notify_one();
                }

                std::unique_ptr<InstructionToken> seed_token(
                        safeAcquire(&incoming_buffer, incoming_lock, incoming_not_empty));
                if (seed_token->api_call != "SET_SEED") {
                    std::cout << "Error handshake sequence" << std::endl;
                    exit(-1);
                }
                std::cout << "Seed received" << std::endl;
                std::memcpy(seed, (uint8_t *) seed_token->payload_content.c_str(), Xceed::Constants::block_size);




                InstructionToken complete_HS_token(nodeE_ID, 0, "T1", "HS_COMPLETE", 1,
                                                   0, 1, "");
                {
                    std::unique_lock<std::mutex> lock{outgoing_lock};
                    outgoing_buffer.push_back(complete_HS_token);
                    out_going_not_empty.notify_one();
                }

                qpp = new Xceed::QPP(seed);
                qpp->setSeed(seed, Xceed::Constants::block_size);

            } else if (acquired->api_call == "ENCRYPT") {

                std::cout << "Encrypt received" << std::endl;
                const int text_size = acquired->payload_size;

                /**
                 * Begin Encryption
                 */

                qpp->setPlainText((uint8_t *) acquired->payload_content.c_str(), text_size);
                uint8_t* cipher = qpp->encrypt();


                std::unique_ptr<InstructionToken> send_encrypted_token(new InstructionToken(0, 0, "T1", "SEND_ID", 1,
                                                                                     0, Xceed::Constants::block_size, std::string((const char *)cipher)));
                /**
                 * Sned SendEncryption
                 */
                {
                    std::unique_lock<std::mutex> lock{outgoing_lock};
                    outgoing_buffer.push_back(*send_encrypted_token);
                    out_going_not_empty.notify_one();
                }

                delete[] cipher;
            } else {
                std::cout << "Message is acquired but API call is undefined" << std::endl;
            }





            // delete cipher;
            // delete reversed;
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
    // delete[] plain_text;

    incoming_thread.join();
    outgoing_thread.join();
    consumer.join();

    return 0;
}
