#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

// Processing buffer size
#define BUFFER_SIZE (11850)

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
    uint16_t msumrBuffer[6][1572];

    // Frame counter
    int frame = 0;

    // This will need some fixes
    unsigned short *imageBufferR = new unsigned short[10000 * 1572];
    unsigned short *imageBufferG = new unsigned short[10000 * 1572];
    unsigned short *imageBufferB = new unsigned short[10000 * 1572];

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        // Convert into 10-bits values
        // 393 byte per channel
        for (int channel = 0; channel < 6; channel++)
        {
            for (int l = 0; l < 393; l++)
            {
                uint8_t pixel_buffer_4[5];
                int pixelpos = 50 + l * 30 + channel * 5;

                pixel_buffer_4[0] = buffer[pixelpos];
                pixel_buffer_4[1] = buffer[pixelpos + 1];
                pixel_buffer_4[2] = buffer[pixelpos + 2];
                pixel_buffer_4[3] = buffer[pixelpos + 3];
                pixel_buffer_4[4] = buffer[pixelpos + 4];

                // Convert 5 bytes to 4 10-bits values
                uint16_t pixel1, pixel2, pixel3, pixel4;
                pixel1 = (pixel_buffer_4[0] << 2) | (pixel_buffer_4[1] >> 6);
                pixel2 = ((pixel_buffer_4[1] % 64) << 4) | (pixel_buffer_4[2] >> 4);
                pixel3 = ((pixel_buffer_4[2] % 16) << 6) | (pixel_buffer_4[3] >> 2);
                pixel4 = ((pixel_buffer_4[3] % 4) << 8) | pixel_buffer_4[4];

                msumrBuffer[channel][l * 4 + 0] = pixel1;
                msumrBuffer[channel][l * 4 + 1] = pixel2;
                msumrBuffer[channel][l * 4 + 2] = pixel3;
                msumrBuffer[channel][l * 4 + 3] = pixel4;
            }
        }

        // Channel R
        for (int i = 0; i < 1572; i++)
        {
            uint16_t pixel = msumrBuffer[2][i];
            imageBufferR[frame * 1572 + i] = pixel * 60;
        }

        // Channel G
        for (int i = 0; i < 1572; i++)
        {
            uint16_t pixel = msumrBuffer[1][i];
            imageBufferG[frame * 1572 + i] = pixel * 60;
        }

        // Channel B
        for (int i = 0; i < 1572; i++)
        {
            uint16_t pixel = msumrBuffer[0][i];
            imageBufferB[frame * 1572 + i] = pixel * 60;
        }

        // Frame counter
        frame++;
    }

    // Print it all out into a .png
    cimg_library::CImg<unsigned short> finalImage(1572, frame, 1, 3);

    cimg_library::CImg<unsigned short> channelImageR(&imageBufferR[0], 1572, frame);
    cimg_library::CImg<unsigned short> channelImageG(&imageBufferG[0], 1572, frame);
    cimg_library::CImg<unsigned short> channelImageB(&imageBufferB[0], 1572, frame);

    finalImage.draw_image(0, 0, 0, 0, channelImageR);
    finalImage.draw_image(0, 0, 0, 1, channelImageG);
    finalImage.draw_image(0, 0, 0, 2, channelImageB);

    finalImage.save_png(argv[2]);

    data_in.close();
}