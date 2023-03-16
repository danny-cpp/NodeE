#include "iostream"
#include "string"


#include "QPP.h"


int main() {

    std::cout << "Hello World" << std::endl;

    int text_size = 12;
    uint8_t* seed = new uint8_t[Xceed::Constants::block_size];
    for (int i = 0; i < Xceed::Constants::block_size; i++) {
         seed[i] = (uint8_t)random();
    }


    uint8_t temp[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0b00100000, 0b01010111, 0b01101111, 0b01110010, 0b01101100, 0b01100100};
    // Expected cipher a8 c7 de e9   66 64 09 fb   eb 41 80 c5

    uint8_t* cipher;
    {
        Xceed::QPP qpp(seed);
        qpp.setPlainText(temp, text_size);
        qpp.setSeed(seed, text_size);
        cipher = qpp.encrypt();
    }

    uint8_t* reversed;
    {
        Xceed::QPP qpp2(seed);
        // uint8_t ciphered[] = {0xa8, 0xc7, 0xde, 0xe9, 0x66, 0x64, 0x09, 0xfb, 0xeb, 0x41, 0x80, 0xc5};
        qpp2.setCipherText(cipher, text_size);
        reversed = qpp2.decrypt();
    }

    delete[] seed;
    delete[] cipher;
    delete[] reversed;

    return 0;
}
