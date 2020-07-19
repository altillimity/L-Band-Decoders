#include <diff.h>

std::vector<uint8_t> FengyunDiff::work(std::vector<uint8_t> in)
{
    std::vector<uint8_t> out;

    for (uint8_t sample : in)
    {
        buffer[0] = buffer[1];
        buffer[1] = sample;

        if (inBuf < 2)
        {
            inBuf++;
            continue;
        }

        Xin_1 = buffer[0] & 0x02;
        Yin_1 = buffer[0] & 0x01;
        Xin = buffer[1] & 0x02;
        Yin = buffer[1] & 0x01;
        if (((Xin >> 1) ^ Yin) == 1)
        {
            Xout = (Yin_1 ^ Yin);
            Yout = (Xin_1 ^ Xin);
            out.push_back((Xout << 1) + (Yout >> 1));
        }
        else
        {
            Xout = (Xin_1 ^ Xin);
            Yout = (Yin_1 ^ Yin);
            out.push_back((Xout + Yout));
        }
    }

    return out;
}