#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

// Processing buffer size
#define BUFFER_SIZE (248 * 2)

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
    uint16_t msumrBuffer[120];

    uint16_t mtvza_buffer[21][120];

    // Frame counter
    int frame = 0;

    // This will need some fixes
    unsigned short *imageBufferR = new unsigned short[10000 * 120];
    unsigned short *imageBufferG = new unsigned short[10000 * 200];
    unsigned short *imageBufferB = new unsigned short[10000 * 200];

    bool done = false;

    // Read until EOF
    while (!data_in.eof())
    {
        //while (!done && !data_in.eof())
        //{
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        if (buffer[5] != std::stoi(argv[3]))
            continue;

        for (int l = 0; l < 120; l += 4)
        {
            int pixelpos = 3 + l;

            msumrBuffer[buffer[5] * 2 + 0] = buffer[pixelpos + 0] << 1 | buffer[pixelpos + 1];
            msumrBuffer[buffer[5] * 2 + 1] = buffer[pixelpos + 2] << 1 | buffer[pixelpos + 3];
            msumrBuffer[l + 2] = buffer[pixelpos * 2 + 4] << 1 | buffer[pixelpos * 2 + 5];
            msumrBuffer[l + 3] = buffer[pixelpos * 2 + 6] << 1 | buffer[pixelpos * 2 + 7];
        }

        //if (buffer[5] > 25)
        //   done = true;
        //}

        // Channel R
        for (int i = 0; i < 120; i++)
        {
            uint16_t pixel = msumrBuffer[i];
            imageBufferR[frame * 120 + i] = pixel * 100;
        }

        // Channel G
        /*for (int i = 0; i < 10; i++)
        {
            uint16_t pixel = msumrBuffer[i];
            imageBufferG[frame * 200 + i] = pixel * 100;
        }

        // Channel B
        for (int i = 0; i < 10; i++)
        {
            uint16_t pixel = msumrBuffer[i];
            imageBufferB[frame * 200 + i] = pixel * 100;
        }
        */
        // Frame counter
        frame++;
        //done = false;
    }

    // Print it all out into a .png
    cimg_library::CImg<unsigned short> finalImage(200, frame, 1, 3);

    cimg_library::CImg<unsigned short> channelImageR(&imageBufferR[0], 120, frame);
    cimg_library::CImg<unsigned short> channelImageG(&imageBufferG[0], 200, frame);
    cimg_library::CImg<unsigned short> channelImageB(&imageBufferB[0], 200, frame);

    finalImage.draw_image(0, 0, 0, 0, channelImageR);
    finalImage.draw_image(0, 0, 0, 1, channelImageG);
    finalImage.draw_image(0, 0, 0, 2, channelImageB);

    channelImageR.save_png(argv[2]);

    data_in.close();
}