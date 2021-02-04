#include "iasi_reader.h"
#include "utils.h"
#include <iostream>

IASIReader::IASIReader()
{
    for (int i = 0; i < 8461; i++)
    {
        channels[i] = new unsigned short[10000 * 30];
    }
    lines = 0;
}

std::vector<uint16_t> repackBitsIASI(uint8_t *in, int dst_bits, int skip, int lengthToConvert)
{
    std::vector<uint16_t> result;
    uint16_t shifter;
    int inShiter = 0;
    for (int i = 0; i < lengthToConvert; i++)
    {
        for (int b = 7; b >= 0; b--)
        {
            if (--skip > 0)
                continue;

            bool bit = getBit<uint8_t>(in[i], b);
            shifter = (shifter << 1 | bit) % (int)pow(2, dst_bits);
            inShiter++;
            if (inShiter == dst_bits)
            {
                result.push_back(shifter);
                inShiter = 0;
            }
        }
    }

    return result;
}

void IASIReader::work(libccsds::CCSDSPacket &packet)
{
    if (packet.payload.size() < 8954)
        return;

    int counter = packet.payload[16];

    //int iis_size = packet.payload[35];

    //std::cout << counter << std::endl;
    //std::cout << iis_size << std::endl;

    int cnt1 = 0, cnt2 = 0;

    if (packet.header.apid == 130)
        cnt1 = 1, cnt2 = 1;
    if (packet.header.apid == 135)
        cnt1 = 0, cnt2 = 1;
    if (packet.header.apid == 140)
        cnt1 = 1, cnt2 = 0;
    if (packet.header.apid == 145)
        cnt1 = 0, cnt2 = 0;

    if (counter <= 30)
    {
        {
            // Decode 8-bits channels
            std::vector<uint16_t> values = repackBitsIASI(&packet.payload.data()[314], 8, 0, 8628);

            for (int channel = 0; channel < 8461; channel++)
                channels[channel][(lines + cnt1) * 60 + (counter - 1) * 2 + cnt2] = values[channel] << 3;
        }

        {
            // Decode 10-bits channels
            //int pos = 328;
            //std::vector<uint16_t> values = repackBits(&packet.payload.data()[pos], 10, 0, 3000);

            //for (int channel = 14; channel < 1000; channel++)
            //    channels[channel][(lines + cnt1) * 60 + (counter - 1) * 2 + cnt2] = values[channel - 14] << 3;
        }
    }

    // Frame counter
    if (counter == 30 && packet.header.apid == 130)
        lines += 2;
}

cimg_library::CImg<unsigned short> IASIReader::getChannel(int channel)
{
    cimg_library::CImg<unsigned short> img = cimg_library::CImg<unsigned short>(channels[channel], 30 * 2, lines);
    img.normalize(0, 65535);
    img.equalize(1000);
    img.mirror('x');
    return img;
}