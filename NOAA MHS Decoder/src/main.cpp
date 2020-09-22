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

int main(int argc, char *argv[])
{
    //Credits "banner"
    std::cout << "---------------------"<<std::endl;
    std::cout << "   NOAA MHS Decoder  " << std::endl;
    std::cout << "      by Zbychu      " << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << std::endl;

    TCLAP::CmdLine cmd("MHS Decoder by Zbychu", ' ', "1.0");

    // Define the arguments
    TCLAP::ValueArg<int> valueChannel("c", "channel", "Channel to extract", true, 0, "channel");
    TCLAP::ValueArg<std::string> valueInput("i", "input", "Raw input file", true, "", "file");
    TCLAP::ValueArg<std::string> valueOutput("o", "output", "Output image file", true, "", "image.png");

    //add the arguments
    cmd.add(valueInput);
    cmd.add(valueOutput);
    cmd.add(valueChannel);

    //parse arguments
    try
    {
        cmd.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e)
    {
        std::cout << e.error() << '\n';
        return 0;
    }
    //get the channel number
    int channel = valueChannel.getValue();
    
    //initalize the data input stream
    std::ifstream data_in(valueInput.getValue(), std::ios::binary);
    //make the buffer
    uint16_t buffer[BUFFER_SIZE / 2];
    //AIP frame count
    int totalAIPFrames = 0;
    //image data
    std::vector<unsigned short> imagebuffer;
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
                        MHSWord[j / 2] = (buffer[104 * i + 78 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 79 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum > 27 && MHSnum < 53)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 46 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 47 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum == 53)
                {
                    for (int j = 0; j < 18; j += 2)
                    {
                        MHSWord[642 + j / 2] = (buffer[104 * i + 46 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 47 + j] % (int)pow(2, 8));
                    }

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        imagebuffer.push_back(MHSWord[j + channel + 49]);
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
                        MHSWord[j / 2] = (buffer[104 * i + 61 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 62 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum > 36 && MHSnum < 80)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 47 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 48 + j] % (int)pow(2, 8));
                    }
                }
                if (MHSnum == 79)
                {

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        imagebuffer.push_back(MHSWord[j + channel + 49]);
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
                    MHSWord[0] = (buffer[104 * i + 95] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 96] % (int)pow(2, 8));
                }
                else if (MHSnum > 0 && MHSnum < 26)
                {
                    for (int j = 0; j < 50; j += 2)
                    {
                        MHSWord[j / 2 + 1 + 25 * MHSnum] = (buffer[104 * i + 47 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 48 + j] % (int)pow(2, 8));
                    }
                }
                else if (MHSnum == 26)
                {
                    for (int j = 0; j < 34; j += 2)
                    {
                        MHSWord[642 + j / 2] = (buffer[104 * i + 47 + j] % (int)pow(2, 8)) << 8 | (buffer[104 * i + 48 + j] % (int)pow(2, 8));
                    }

                    for (int j = 0; j < 540; j += 6)
                    {
                        //idk why I needed to add +49 there, but doesn't work without it
                        imagebuffer.push_back(MHSWord[j + channel + 49]);
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
    cimg_library::CImg<unsigned short> outputImage(&imagebuffer[0], 90, line + 1);
    //save the image
    outputImage.save_bmp(valueOutput.getValue().c_str());
}