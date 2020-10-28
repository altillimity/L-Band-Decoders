#pragma once

#include <ccsds/ccsds.h>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

class AVHRRReader
{
private:
    unsigned short *channels[5];

public:
    AVHRRReader();
    int lines;
    void work(uint16_t* buffer);
    cimg_library::CImg<unsigned short> getChannel(int channel);
};