#pragma once

#include "string"
#include "vector"
#include "array"

#include "Constants.h"


namespace Xceed {

    using namespace Constants;

    class QPP {
    public:
        QPP();

        ~QPP() {
            delete[] bit_rep_plain_text;
            delete[] bit_rep_cipher_text;
            delete[] seed;
        }

        uint8_t* encrypt();
        uint8_t* decrypt();

        void setPlainText(std::string &plain_text);
        void setPlainText(const uint8_t* plain_text, int in_text_size);

        void setCipherText(const uint8_t* cipher_text, int in_text_size);

        void setSeed(const uint8_t* in_seed, int seed_size);

        int getStringLength();

    protected:
        void generateMatrix();
        void generateInvMatrix();

    private:
        int text_size;

        // uint8_t pmat_list[M][mat_size];
        std::vector<std::vector<uint8_t>> pmat_list;
        // uint8_t inv_pmat_list[M][mat_size];
        std::vector<std::vector<uint8_t>> inv_pmat_list;

        uint8_t* bit_rep_plain_text;
        uint8_t* bit_rep_cipher_text;
        uint8_t* seed;
    };
}
