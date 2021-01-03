#include <iostream>
#include <fstream>
#include <vector>
#define cimg_use_png
#define cimg_display 0
#include "CImg.h"
#include "tclap/CmdLine.h"

// Processing buffer size
#define BUFFER_SIZE (11090 * 2)

// Returns the asked bit!
template <typename T>
inline bool getBit(T &data, int bit)
{
    return (data >> bit) & 1;
}

unsigned short *channels[5];

int main(int argc, char *argv[])
{
    //Credits "banner"
    std::cout << "---------------------"<<std::endl;
    std::cout << "   NOAA MHS Decoder  " << std::endl;
    std::cout << "      by Zbychu      " << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << std::endl;

    //initalize the data input stream
    std::ifstream data_in(argv[1], std::ios::binary);
    //make the buffer
    uint16_t buffer[BUFFER_SIZE / 2];
    //AIP frame count
    int totalAIPFrames = 0;
    //image data
    for (int i = 0; i < 5; i++)
        channels[i] = new unsigned short[10000 * 90];
    //line vount
    int line = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        //get the HRPT minor frame number
        bool bit1 = getBit(buffer[6], 8);
        bool bit2 = getBit(buffer[6], 7);
        int frmNum = bit1 << 1 | bit2;
        //init the MHS buffer
        uint16_t MHSWord[643];

        if (frmNum == 3)
        {
            for (int i = 1; i < 6; i++)
            {
                //get the MIU minor cycle count
                uint16_t MHSnum = buffer[104 * i + 6];
                MHSnum >>= 2;

                //
                //second packet start (PKT 0)
                //

                //read the MHS Data from AIP frames (one of 3 in each AIP frame)
                if (MHSnum == 27)
                {
                    for (int j = 0; j < 18; j += 2)
                    {
                        MHSWord[j / 2] = (buffer[104 * i + 79 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 78 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum > 27 && MHSnum < 53)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 47 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 46 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum == 53)
                {
                    for (int j = 0; j < 18; j += 2)
                    {
                        MHSWord[642 + j / 2] = (buffer[104 * i + 47 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 46 + j] % (int)pow(2, 8));
                    }

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        for(int k = 0; k<5; k++)
                            channels[k][line * 90 + 90 - j / 6] = MHSWord[j + k + 50];
                    }
                    line = line + 1;
                }

                //
                //second packet end (PKT 0)
                //

                //
                //third packet start (PKT 1)
                //

                //read the MHS Data from AIP frames (one of 3 in each AIP frame)
                if (MHSnum == 54)
                {
                    for (int j = 0; j < 36; j += 2)
                    {
                        MHSWord[j / 2] = (buffer[104 * i + 62 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 61 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum > 36 && MHSnum < 80)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 48 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 47 + j] % (int)pow(2, 8));
                    }
                }
                if (MHSnum == 79)
                {

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        for (int k = 0; k < 5; k++)
                            channels[k][line * 90 + 90 - j / 6] = MHSWord[j + k + 50];
                    }

                    line = line + 1;
                }

                //
                //third packet end (PKT 1)
                //

                //
                //first packet start (PKT 2)
                //

                //read the MHS Data from AIP frames (one of 3 in each AIP frame)
                if (MHSnum == 0)
                {
                    std::fill_n(MHSWord, 643, 0);
                    MHSWord[0] = (buffer[104 * i + 96] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 95] % (int)pow(2, 8));
                }
                else if (MHSnum > 0 && MHSnum < 26)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 48 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 47 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum == 26)
                {
                    for (int j = 0; j < 34; j += 2)
                    {
                        MHSWord[642 + j / 2] = (buffer[104 * i + 48 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 47 + j] % (int)pow(2, 8));
                    }

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        for (int k = 0; k < 5; k++)
                            channels[k][line * 90 + 90 - j / 6] = MHSWord[j + k + 50];
                    }

                    line = line + 1;
                }

                //
                //first packet end (PKT 2)
                //

                //AIP frame counter
                totalAIPFrames++;
            }
        }
    }
    //close the input stream
    data_in.close();
    //some feedback to the user
    std::cout<<"Found "<<totalAIPFrames<<" AIP Frames."<<std::endl;
    std::cout<<"Found "<<line<<" MHS lines."<<std::endl;
    std::cout<<std::endl;

    //create an image
    cimg_library::CImg<unsigned short> channel1(channels[0], 90, line + 1, 1, 1);
    cimg_library::CImg<unsigned short> channel2(channels[1], 90, line + 1, 1, 1);
    cimg_library::CImg<unsigned short> channel3(channels[2], 90, line + 1, 1, 1);
    cimg_library::CImg<unsigned short> channel4(channels[3], 90, line + 1, 1, 1);
    cimg_library::CImg<unsigned short> channel5(channels[4], 90, line + 1, 1, 1);
    cimg_library::CImg<unsigned short> imgAll(270, (line + 1) * 2);
    imgAll.fill(0);
    

    channel1.equalize(1000);
    channel2.equalize(1000);
    channel3.equalize(1000);
    channel4.equalize(1000);
    channel5.equalize(1000);
    

    imgAll.draw_image(0, 0, channel1);
    imgAll.draw_image(90, 0, channel2);
    imgAll.draw_image(180, 0, channel3);
    imgAll.draw_image(0, line+1, channel4);
    imgAll.draw_image(90, line+1, channel5);
    //save the image
    
    channel1.save_png("MHS-1.png");
    channel2.save_png("MHS-2.png");
    channel3.save_png("MHS-3.png");
    channel4.save_png("MHS-4.png");
    channel5.save_png("MHS-5.png");
    imgAll.save_png("MHS-All.png");
    
}