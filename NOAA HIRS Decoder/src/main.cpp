#include <iostream>
#include <fstream>
#include <vector>
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"
#include "TIP_reader.h"

int main(int argc, char *argv[])
{
    TIPReader tipreader;
    std::ofstream out("out.bin");
    std::vector<std::array<uint8_t, 104>> tipFrames = tipreader.readTIP(std::ifstream(argv[1], std::ios::binary));
    for (int i = 0; i < tipFrames.size(); i++)
    {
        out.write((char *)&tipFrames[i], 104);
    }
    out.close();
}