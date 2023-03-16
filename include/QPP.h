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
            delete bit_rep_plain_text;
            delete bit_rep_cipher_text;
            delete seed;
        }

        uint8_t* encrypt();

        void setPlainText(std::string &plain_text);
        void setPlainText(const uint8_t* plain_text, int in_text_size);

        void setSeed(const uint8_t* in_seed, int seed_size);

        int getStringLength();

    protected:
        void generateMatrix();

    private:
        int text_size;
        int steps;

        uint8_t pmat_list[M][mat_size];

        uint8_t* bit_rep_plain_text;
        uint8_t* bit_rep_cipher_text;
        uint8_t* seed;
    };
}
