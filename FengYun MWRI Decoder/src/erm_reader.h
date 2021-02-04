#pragma once

#include <ccsds/ccsds.h>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

class ERMImage
{
public:
    unsigned short imageData[151 * 32];
    cimg_library::CImg<unsigned short> getImage();
    int mk = -1;
    int lastMkMatch;
};

class ERMReader
{
private:
    unsigned short *imageData;
    unsigned short *staticChannel;
    std::vector<ERMImage> imageVector;

public:
    ERMReader();
    int lines;
    void work(libccsds::CCSDSPacket &packet);
    cimg_library::CImg<unsigned short> getChannel(int channel);
};