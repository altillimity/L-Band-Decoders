#include "derandomizer.h"

namespace libccsds
{
    Derandomizer::Derandomizer()
    {
        // From gr-poes-weather (including comments!)
        unsigned char feedbk, randm = 0xff;
        // Original Polynomial is :  1 + x3 + x5 + x7 +x8
        d_rantab[0] = 0;
        d_rantab[1] = 0;
        d_rantab[2] = 0;
        d_rantab[3] = 0;
        for (int i = 4; i < CADU_SIZE; i++)
        { //4ASM bytes + 1020bytes = 32 + 8160 bits in CADU packet

            d_rantab[i] = 0;
            for (int j = 0; j <= 7; j++)
            {
                d_rantab[i] = d_rantab[i] << 1;
                if (randm & 0x80) //80h = 1000 0000b
                    d_rantab[i]++;

                //Bit-Wise AND between: Fixed shift register(95h) and the state of the
                // feedback register: randm
                feedbk = randm & 0x95; //95h = 1001 0101--> bits 1,3,5,8
                //feedback contains the contents of the registers masked by the polynomial
                //  1 + x3 + x5 + xt +x8 = 95 h
                randm = randm << 1;

                if ((((feedbk & 0x80) ^ (0x80 & feedbk << 3)) ^ (0x80 & (feedbk << 5))) ^ (0x80 & (feedbk << 7)))
                    randm++;
            }
        }
    }

    CADU Derandomizer::work(CADU &input)
    {
        CADU output;
        for (int i = 4; i < CADU_SIZE; i++)
        {
            output[i] = input[i] ^ d_rantab[i];
        }
        return output;
    }
} // namespace libccsds