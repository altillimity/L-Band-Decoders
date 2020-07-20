#include <diff.h>

std::vector<uint8_t> FengyunDiff::work(std::vector<uint8_t>& in)
{
    std::vector<uint8_t> out;

    // Process all given samples
    for (uint8_t sample : in)
    {
        // Push new sample into buffer
        buffer[0] = buffer[1];
        buffer[1] = sample;

        // We need at least 2
        if (inBuf < 2)
        {
            inBuf++;
            continue;
        }

        // Perform differential decoding
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