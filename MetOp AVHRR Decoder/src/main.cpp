#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

// Processing buffer size
#define BUFFER_SIZE (12966)

// Returns the asked bit!
template <typename T>
inline bool getBit(T &data, int &bit)
{
    return (data >> bit) & 1;
}

int main(int argc, char *argv[])
{
    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);

    // Read buffer
    uint8_t buffer[BUFFER_SIZE];

    // AVHRR Packet buffer
    uint16_t avhrrBuffer[12944];

    // Frame counter
    int frame = 0;

    // This will need some fixes
    unsigned short *imageBufferR = new unsigned short[10000 * 2048];
    unsigned short *imageBufferG = new unsigned short[10000 * 2048];
    unsigned short *imageBufferB = new unsigned short[10000 * 2048];

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        // Print APID
        int apid = ((buffer[0] % ((int)pow(2, 3))) << 8) | buffer[1];
        std::cout << "APID : " << apid << std::endl;

        int pos = 20; // AVHRR Data, eg, User Data Field

        // Convert into 10-bits values
        for (int i = 0; i < 12944; i += 4)
        {
            avhrrBuffer[i] = (buffer[pos + 0] << 2) | (buffer[pos + 1] >> 6);
            avhrrBuffer[i + 1] = ((buffer[pos + 1] % 64) << 4) | (buffer[pos + 2] >> 4);
            avhrrBuffer[i + 2] = ((buffer[pos + 2] % 16) << 6) | (buffer[pos + 3] >> 2);
            avhrrBuffer[i + 3] = ((buffer[pos + 3] % 4) << 8) | buffer[pos + 4];
            pos += 5;
        }

        // Channel R
        for (int i = 0; i < 2048; i++)
        {
            uint16_t pixel = avhrrBuffer[55 + 1 + i * 5];
            imageBufferR[frame * 2048 + i] = pixel * 60;
        }

        // Channel G
        for (int i = 0; i < 2048; i++)
        {
            uint16_t pixel = avhrrBuffer[55 + 1 + i * 5];
            imageBufferG[frame * 2048 + i] = pixel * 60;
        }

        // Channel B
        for (int i = 0; i < 2048; i++)
        {
            uint16_t pixel = avhrrBuffer[55 + 0 + i * 5];
            imageBufferB[frame * 2048 + i] = pixel * 60;
        }

        // Frame counter
        frame++;
    }

    // Print it all out into a .png
    cimg_library::CImg<unsigned short> finalImage(2048, frame, 1, 3);

    cimg_library::CImg<unsigned short> channelImageR(&imageBufferR[0], 2048, frame);
    cimg_library::CImg<unsigned short> channelImageG(&imageBufferG[0], 2048, frame);
    cimg_library::CImg<unsigned short> channelImageB(&imageBufferB[0], 2048, frame);

    finalImage.draw_image(0, 0, 0, 0, channelImageR);
    finalImage.draw_image(0, 0, 0, 1, channelImageG);
    finalImage.draw_image(0, 0, 0, 2, channelImageB);

    finalImage.save_png(argv[2]);

    data_in.close();
}