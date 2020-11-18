#pragma once

#include "cadu.h"

namespace libccsds
{
    /*
        Simple CCSDS derandomizer
    */
    class Derandomizer
    {
    private:
        uint8_t d_rantab[CADU_SIZE]; // Derandomization table

    public:
        Derandomizer();
        CADU work(CADU &input); // Main function
    };
} // namespace libccsds