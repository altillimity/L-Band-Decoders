#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "msumr_reader.h"

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
    uint8_t buffer[11850];

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "   METEOR MSU-MR Decoder" << std::endl;
    std::cout << "         by Aang23" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Processing..." << std::endl;

    MSUMRReader reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, 11850);

        reader.work(buffer);

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "MSU-MR Lines            : " << reader.lines << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image1 = reader.getChannel(0);
    cimg_library::CImg<unsigned short> image2 = reader.getChannel(1);
    cimg_library::CImg<unsigned short> image3 = reader.getChannel(2);
    cimg_library::CImg<unsigned short> image4 = reader.getChannel(3);
    cimg_library::CImg<unsigned short> image5 = reader.getChannel(4);
    cimg_library::CImg<unsigned short> image6 = reader.getChannel(5);

    std::cout << "Channel 1..." << std::endl;
    image1.save_png("MSU-MR-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("MSU-MR-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("MSU-MR-3.png");

    std::cout << "Channel 4..." << std::endl;
    image4.save_png("MSU-MR-4.png");

    std::cout << "Channel 5..." << std::endl;
    image5.save_png("MSU-MR-5.png");

    std::cout << "Channel 6..." << std::endl;
    image5.save_png("MSU-MR-6.png");

    std::cout << "221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(1572, reader.lines, 1, 3);
    {
        image221.draw_image(0, 0, 0, 0, image2);
        image221.draw_image(0, 0, 0, 1, image2);
        image221.draw_image(0, 0, 0, 2, image1);
    }
    image221.save_png("MSU-MR-RGB-221.png");
    image221.equalize(1000);
    image221.normalize(0, std::numeric_limits<unsigned char>::max());
    image221.save_png("MSU-MR-RGB-221-EQU.png");

    std::cout << "321 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image321(1572, reader.lines, 1, 3);
    {
        image321.draw_image(0, 0, 0, 0, image3);
        image321.draw_image(0, 0, 0, 1, image2);
        image321.draw_image(0, 0, 0, 2, image1);
    }
    image321.save_png("MSU-MR-RGB-321.png");
    image321.equalize(1000);
    image321.normalize(0, std::numeric_limits<unsigned char>::max());
    image321.save_png("MSU-MR-RGB-321-EQU.png");

    data_in.close();
}