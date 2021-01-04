#pragma once

#include <ccsds/ccsds.h>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

class HIRSReader
{
private:
    unsigned short *channels[20];
    uint16_t lineBuffer[12944];

public:
    HIRSReader();
    int lines;
    void work(libccsds::CCSDSPacket &packet);
    cimg_library::CImg<unsigned short> getChannel(int channel);
};