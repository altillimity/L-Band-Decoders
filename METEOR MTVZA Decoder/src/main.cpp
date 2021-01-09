#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "mtvza_reader.h"

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// IO files
std::ifstream data_in;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " inputFrames.bin" << std::endl;
        return 0;
    }

    // Read buffer
    uint8_t buffer[248];

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "   METEOR MTVZA Decoder" << std::endl;
    std::cout << "         by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Processing..." << std::endl;

    MTVZAReader reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, 248);

        reader.work(buffer);

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "MTVZA Lines            : " << reader.lines << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    std::cout << "Global Composite..." << std::endl;
    int all_width_count = 20;
    int all_height_count = 6;
    cimg_library::CImg<unsigned short> imageAll(26 * 5 * all_width_count, reader.getChannel(0).height() * all_height_count, 1, 1);
    {
        int height = reader.getChannel(0).height();

        for (int row = 0; row < all_height_count; row++)
        {
            for (int column = 0; column < all_width_count; column++)
            {
                if (row * all_width_count + column >= 8461)
                    break;

                imageAll.draw_image(26 * 5 * column, height * row, 0, 0, reader.getChannel(row * all_width_count + column));
            }
        }
    }
    imageAll.save_png("MTVZA-ALL.png");

    data_in.close();
}