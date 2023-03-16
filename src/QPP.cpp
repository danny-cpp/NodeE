#include "QPP.h"

#include <cstring>

using namespace Xceed::Constants;

Xceed::QPP::QPP() {
    generateMatrix();
}

uint8_t* Xceed::QPP::encrypt() {

    for (int i = 0, byte_ptr = 0; i < text_size; i++, byte_ptr++) {

        for (int right_side_offset = 8 - n, j = 0; right_side_offset >= 0; right_side_offset -= n, j++) {

            uint8_t cipher_byte_mask = 0;
            int moving_mask = mask >> (n * j);
            int dispatch_index = ((i * right_side_offset) + j) % M;

            // Extracting
            int basis_select = (*(bit_rep_plain_text + byte_ptr) & moving_mask) >> right_side_offset;

            // Writing byte
            uint8_t acquired_value = pmat_list[dispatch_index][basis_select];
            cipher_byte_mask = (uint8_t) acquired_value << right_side_offset;

            // Writing cipher
            *(bit_rep_cipher_text + byte_ptr) &= ~moving_mask;
            *(bit_rep_cipher_text + byte_ptr) |= (cipher_byte_mask & moving_mask);

        }
    }

    return bit_rep_cipher_text;
}

void Xceed::QPP::generateMatrix() {

    uint8_t m1[mat_size] = {1, 2, 0, 3};
    uint8_t m2[mat_size] = {2, 1, 0, 3};
    uint8_t m3[mat_size] = {1, 3, 2, 0};
    uint8_t m4[mat_size] = {0, 3, 2, 1};
    uint8_t m5[mat_size] = {0, 3, 1, 2};

    std::memcpy(pmat_list[0], m1, sizeof(uint8_t)*mat_size);
    std::memcpy(pmat_list[1], m2, sizeof(uint8_t)*mat_size);
    std::memcpy(pmat_list[2], m3, sizeof(uint8_t)*mat_size);
    std::memcpy(pmat_list[3], m4, sizeof(uint8_t)*mat_size);
    std::memcpy(pmat_list[4], m5, sizeof(uint8_t)*mat_size);
}

int Xceed::QPP::getStringLength() {
    return text_size;
}

void Xceed::QPP::setPlainText(std::string &plain_text) {

    bit_rep_plain_text = new uint8_t[plain_text.size()];
    bit_rep_cipher_text = new uint8_t[plain_text.size()];

    std::memcpy(bit_rep_plain_text, plain_text.c_str(), plain_text.size());
    this->text_size = (int)plain_text.size();
}

void Xceed::QPP::setPlainText(const uint8_t *plain_text, int in_text_size) {

    bit_rep_plain_text = new uint8_t[in_text_size];
    bit_rep_cipher_text = new uint8_t[in_text_size];

    std::memcpy(bit_rep_plain_text, plain_text, in_text_size);
    this->text_size = in_text_size;

}

void Xceed::QPP::setSeed(const uint8_t* in_seed, int seed_size) {

    seed = new uint8_t [seed_size];
    text_size = seed_size;

    std::memcpy(seed, in_seed, seed_size);
}
