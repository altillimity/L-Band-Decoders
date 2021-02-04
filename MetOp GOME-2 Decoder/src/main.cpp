#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "gome_reader.h"
#include <filesystem>

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
    if (argc != 2 && argc != 3)
    {
        std::cout << "Usage : " << argv[0] << " inputFrames.bin" << std::endl;
        return 0;
    }

    bool write_all = false;

    // Terra mode?
    if (argc == 3)
    {
        if (std::string(argv[2]) == "-a")
            write_all = true;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t gome_cadu = 0, ccsds = 0, gome_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "  GOME Decoder by Aang23" << std::endl;
    std::cout << "           MetOp" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer(882, true);

    // Readers
    GOMEReader gome_reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 12 is MHS)
        if (vcdu.vcid == 24)
        {
            gome_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 384)
                {
                    std::cout << pkt.payload.size() << std::endl;
                    gome_reader.work(pkt);
                    gome_ccsds++;
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 24 (gGOME) Frames : " << gome_cadu << std::endl;
    std::cout << "CCSDS Frames          : " << ccsds << std::endl;
    std::cout << "GOME Frames           : " << gome_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    if (write_all)
    {
        if (!std::filesystem::exists("GOME_ALL"))
            std::filesystem::create_directory("GOME_ALL");
        for (int i = 0; i < 6144; i++)
        {
            std::cout << "Channel " << (i + 1) << std::endl;
            gome_reader.getChannel(i).save_png(std::string("GOME_ALL/GOME-" + std::to_string(i + 1) + ".png").c_str());
        }
    }

    //std::cout << "Channel IR imaging..." << std::endl;
    gome_reader.getChannel(1).save_png(std::string("GOME.png").c_str());

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    int all_width_count = 130;
    int all_height_count = 48;
    cimg_library::CImg<unsigned short> imageAll(30 * all_width_count, gome_reader.getChannel(0).height() * all_height_count, 1, 1);
    {
        int height = gome_reader.getChannel(0).height();

        for (int row = 0; row < all_height_count; row++)
        {
            for (int column = 0; column < all_width_count; column++)
            {
                if (row * all_width_count + column >= 6144)
                    break;

                imageAll.draw_image(30 * column, height * row, 0, 0, gome_reader.getChannel(row * all_width_count + column));
            }
        }
    }
    imageAll.save_png("GOME-2-ALL.png");

    data_in.close();
}
