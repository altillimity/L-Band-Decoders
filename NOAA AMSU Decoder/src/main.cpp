#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "noaa_frame_reader.h"
#include "simpledeframer.h"
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"

int main(int argc, char *argv[])
{
    std::cout << "---------------------" << std::endl;
    std::cout << " NOAA AMSU-A Decoder " << std::endl;
    std::cout << "      by Zbychu      " << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << std::endl;

    NOAAFrameReader aipreader;
    std::vector<std::array<uint8_t, 104>> aipFrames = aipreader.readFrames(std::ifstream(argv[1], std::ios::binary), 3);
    std::vector<uint8_t> amsuA2words;
    std::vector<uint8_t> amsuA1words;

    for (int i = 0; i < aipFrames.size(); i++)
    {
        std::array<uint8_t, 104> buffer = aipFrames[i];
        for (int j = 0; j < 14; j += 2)
        {
            if (buffer[j + 34] << 8 | buffer[j + 35] != 1)
            {
                amsuA2words.push_back(buffer[j + 34]);
                amsuA2words.push_back(buffer[j + 35]);
            }
        }
        for (int j = 0; j < 26; j += 2)
        {
            if (buffer[j + 8] << 8 | buffer[j + 9] != 1)
            {
                amsuA1words.push_back(buffer[j + 8]);
                amsuA1words.push_back(buffer[j + 9]);
            }
        }
        //aipout.write((char *)&aipFrames[i][0], 104);
    }
    SimpleDeframer<uint32_t, 24, 312 * 8, 0xFFFFFF> amsuDeframer;
    SimpleDeframer<uint32_t, 24, 1240 * 8, 0xFFFFFF> amsuA1Deframer;
    std::vector<std::vector<uint8_t>> amsuA2Data = amsuDeframer.work(amsuA2words);
    std::vector<std::vector<uint8_t>> amsuA1Data = amsuA1Deframer.work(amsuA1words);

    uint16_t image_bufferA2[2][30 * amsuA2Data.size()];
    uint16_t image_bufferA1[13][30 * amsuA1Data.size()];

    int linesA2 = 0;
    for (std::vector<uint8_t> frame : amsuA2Data)
    {
        for (int i = 0; i < 240; i += 8)
        {
            image_bufferA2[0][30 * linesA2 + i / 8] = frame[i + 12] << 8 | frame[12 + i + 1];
            image_bufferA2[1][30 * linesA2 + i / 8] = frame[i + 12 + 2] << 8 | frame[12 + i + 1 + 2];
        }
        linesA2++;
    }

    int linesA1 = 0;
    for (std::vector<uint8_t> frame : amsuA1Data)
    {
        for (int i = 0; i < 1020; i += 34)
        {
            for (int j = 0; j < 13; j++)
            {
                image_bufferA1[j][30 * linesA1 + i / 34] = (frame[i + 16 + 2 * j] << 8) | frame[16 + i + 2 * j + 1];
            }
        }
        linesA1++;
    }

    cimg_library::CImg<uint16_t> allimg(240, 2 * linesA1, 1, 1);

    cimg_library::CImg<uint16_t> channel1(&image_bufferA2[0][0], 30, linesA2, 1, 1);
    channel1.equalize(1000);
    channel1.save_png("AMSU-A2-1.png");
    allimg.draw_image(channel1);

    cimg_library::CImg<uint16_t> channel2(&image_bufferA2[1][0], 30, linesA2, 1, 1);
    channel2.equalize(1000);
    channel2.save_png("AMSU-A2-2.png");
    allimg.draw_image(30, 0, channel2);

    for (int i = 0; i < 13; i++){
        cimg_library::CImg<uint16_t> channel(&image_bufferA1[i][0], 30, linesA1, 1, 1);
        channel.equalize(1000);
        channel.save_png(("AMSU-A1-" + std::to_string(i+3) +".png").c_str());
        allimg.draw_image((i+2) % 8 * 30, (i+2)/8*linesA1, channel);
    }
    allimg.save_png("AMSU-all.png");
    
}