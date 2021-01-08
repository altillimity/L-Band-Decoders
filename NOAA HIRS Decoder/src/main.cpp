#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"
#include "noaa_frame_reader.h"
#include <math.h>

int main(int argc, char *argv[])
{

    std::cout << "---------------------" << std::endl;
    std::cout << "  NOAA HIRS Decoder  " << std::endl;
    std::cout << "      by Zbychu      " << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << std::endl;

    const int HIRSPositions[36] = {16, 17, 22, 23, 26, 27, 30, 31, 34, 35, 38, 39, 42, 43, 54, 55, 58, 59, 62, 63, 66, 67, 70, 71, 74, 75, 78, 79, 82, 83, 84, 85, 88, 89, 92, 93};
    std::vector<std::array<uint8_t, 104>> tipFrames;

    std::string arg2;
    if (argv[2] != NULL)
    {
        arg2 = argv[2];
    }
    else
    {
        arg2 = "";
    }

    if (arg2 == "dsb")
    {
        std::cout << "Reading raw TIP frames..." << std::endl;
        std::ifstream data_in(argv[1], std::ios::binary);
        uint8_t buffer[104];
        std::array<uint8_t, 104> arrayBuffer;
        while (!data_in.eof())
        {
            // Read buffer
            data_in.read((char *)buffer, 104);
            std::copy(std::begin(buffer), std::end(buffer), std::begin(arrayBuffer));
            tipFrames.push_back(arrayBuffer);
        }
        data_in.close();
    }
    else
    {
        std::cout << "Reading HRPT frames" << '\n'
                  << std::endl;
        NOAAFrameReader tipreader;
        tipFrames = tipreader.readFrames(std::ifstream(argv[1], std::ios::binary), 1);
    }

    uint8_t HIRS_data[tipFrames.size()][36];
    int line = 0;
    uint16_t imageBuffer[20][56][1000];
    for (int i = 0; i < tipFrames.size(); i++)
    {
        int pos = 0;
        for (int j : HIRSPositions)
        {
            HIRS_data[i][pos] = tipFrames[i][j];
            pos++;
            //std::cout<<pos<<std::endl;
        }
        uint16_t enct = ((HIRS_data[i][2] % (int)pow(2, 5)) << 1) | (HIRS_data[i][3] >> 7);
        //std::cout << "element number:" << enct << " encoder position:" << (unsigned int)HIRS_data[i][0] << std::endl;

        if (enct + 1 == (uint8_t)HIRS_data[i][0] && enct < 56)
        {
            imageBuffer[0][enct][line] = (HIRS_data[i][3] & 0b00111111) << 7 | HIRS_data[i][4] >> 1;
            imageBuffer[16][enct][line] = (HIRS_data[i][4] & 0b00000001) << 12 | HIRS_data[i][5] << 4 | (HIRS_data[i][6] & 0b11110000) >> 4;
            imageBuffer[1][enct][line] = (HIRS_data[i][6] & 0b00001111) << 9 | HIRS_data[i][7] << 1 | (HIRS_data[i][8] & 0b10000000) >> 7;
            imageBuffer[2][enct][line] = (HIRS_data[i][8] & 0b01111111) << 6 | (HIRS_data[i][9] & 0b11111100) >> 2;
            imageBuffer[12][enct][line] = (HIRS_data[i][9] & 0b00000011) << 11 | HIRS_data[i][10] << 3 | (HIRS_data[i][11] & 0b11100000) >> 5;
            imageBuffer[3][enct][line] = (HIRS_data[i][11] & 0b00011111) << 8 | HIRS_data[i][12];
            imageBuffer[17][enct][line] = HIRS_data[i][13] << 5 | (HIRS_data[i][14] & 0b11111000) >> 5;
            imageBuffer[10][enct][line] = (HIRS_data[i][14] & 0b00000111) << 10 | HIRS_data[i][15] << 2 | (HIRS_data[i][16] & 0b11000000) >> 6;
            imageBuffer[18][enct][line] = (HIRS_data[i][16] & 0b00111111) << 7 | HIRS_data[i][17] >> 1;
            imageBuffer[6][enct][line] = (HIRS_data[i][17] & 0b10000000) << 12 | HIRS_data[i][18] << 4 | (HIRS_data[i][19] & 0b11110000) >> 4;
            imageBuffer[7][enct][line] = (HIRS_data[i][19] & 0b00001111) << 9 | HIRS_data[i][20] << 1 | (HIRS_data[i][21] & 0b10000000) >> 7;
            imageBuffer[19][enct][line] = (HIRS_data[i][21] & 0b01111111) << 6 | (HIRS_data[i][22] & 0b11111100) >> 2;
            imageBuffer[9][enct][line] = (HIRS_data[i][22] & 0b00000011) << 11 | HIRS_data[i][23] << 3 | (HIRS_data[i][24] & 0b11100000) >> 5;
            imageBuffer[13][enct][line] = (HIRS_data[i][24] & 0b00011111) << 8 | HIRS_data[i][25];
            imageBuffer[5][enct][line] = HIRS_data[i][26] << 5 | (HIRS_data[i][27] & 0b11111000) >> 5;
            imageBuffer[4][enct][line] = (HIRS_data[i][27] & 0b00000111) << 10 | HIRS_data[i][28] << 2 | (HIRS_data[i][29] & 0b11000000) >> 6;
            imageBuffer[14][enct][line] = (HIRS_data[i][29] & 0b00111111) << 7 | HIRS_data[i][30] >> 1;
            imageBuffer[11][enct][line] = (HIRS_data[i][30] & 0b10000000) << 12 | HIRS_data[i][31] << 4 | (HIRS_data[i][32] & 0b11110000) >> 4;
            imageBuffer[15][enct][line] = (HIRS_data[i][32] & 0b00001111) << 9 | HIRS_data[i][33] << 1 | (HIRS_data[i][34] & 0b10000000) >> 7;
            imageBuffer[8][enct][line] = (HIRS_data[i][34] & 0b01111111) << 6 | (HIRS_data[i][35] & 0b11111100) >> 2;

            unsigned int next;
            if (tipFrames.size() > i)
            {
                next = ((tipFrames[i + 1][22] % (int)pow(2, 5)) << 1) | (tipFrames[i + 1][23] >> 7);
            }
            else
                next = 56;

            if (enct + 1 > 55 && next > 55)
            {
                line++;
            }
        }
    }
    for (int channel = 0; channel < 20; channel++)
    {
        for (int x = 0; x < 56; x++)
        {
            for (int ln = 0; ln < line; ln++)
            {
                if ((imageBuffer[channel][x][ln] >> 12) == 1)
                {
                    imageBuffer[channel][x][ln] = (imageBuffer[channel][x][ln] & 0b0000111111111111) + 4095;
                }
                else
                {
                    int buffer = 4096 - ((imageBuffer[channel][x][ln] & 0b0000111111111111));
                    imageBuffer[channel][x][ln] = abs(buffer);
                }
            }
        }
    }

    std::cout << "TIP Frames found  : " << tipFrames.size() << std::endl;
    std::cout << "HIRS Lines foud   : " << line << std::endl;
    std::vector<unsigned short> channelBuffer;

    cimg_library::CImg<unsigned short> allchannels(280, line * 4);

    for (int channel = 0; channel < 20; channel++)
    {
        channelBuffer.clear();
        for (int ln = 0; ln < line; ln++)
        {
            for (int x = 0; x < 56; x++)
            {
                channelBuffer.push_back(imageBuffer[channel][x][ln]);
            }
        }
        cimg_library::CImg<unsigned short> channelbuff(&channelBuffer[0], 56, line, 1, 1);
        channelbuff.normalize(0, 65535);
        channelbuff.save_png(("HIRS-" + std::to_string(channel + 1) + ".png").c_str());
        allchannels.draw_image(56 * (channel % 5), (channel / 5) * line, channelbuff);
    }
    allchannels.save_png("HIRS-All.png");
}