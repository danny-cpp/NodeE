#include "iostream"
#include "string"


#include "QPP.h"


int main() {

    std::cout << "Hello World" << std::endl;

    int text_size = 12;
    std::vector<uint8_t> seed_vec;
    for (int i = 0; i < text_size; i++) {
         seed_vec.push_back((uint8_t)random());
    }
    uint8_t* seed = &seed_vec[0];

    uint8_t temp[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0b00100000, 0b01010111, 0b01101111, 0b01110010, 0b01101100, 0b01100100};

    Xceed::QPP qpp;
    qpp.setPlainText(temp, text_size);
    qpp.setSeed(seed, text_size);
    uint8_t* cipher = qpp.encrypt();


    return 0;
}
