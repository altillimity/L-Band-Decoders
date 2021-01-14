#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include <filesystem>
#include "iasi_imaging_reader.h"
#include "iasi_reader.h"

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
        std::cout << "Usage : " << argv[0] << " inputFrames.bin [-a]" << std::endl;
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
    uint64_t iasi_cadu = 0, ccsds = 0, iasi_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "  IASI Decoder by Aang23" << std::endl;
    std::cout << "           MetOp" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer(882, true);

    // Readers
    IASIReader iasireader;
    IASIIMGReader iasireader_img;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 10 is IASI)
        if (vcdu.vcid == 10)
        {
            iasi_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 150)
                {
                    iasireader_img.work(pkt);
                    iasi_ccsds++;
                }
                else if (pkt.header.apid == 130 || pkt.header.apid == 135 || pkt.header.apid == 140 || pkt.header.apid == 145)
                {
                    iasireader.work(pkt);
                    iasi_ccsds++;
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 10 (IASI) Frames : " << iasi_cadu << std::endl;
    std::cout << "CCSDS Frames          : " << ccsds << std::endl;
    std::cout << "IASI Frames           : " << iasi_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    if (write_all)
    {
        if (!std::filesystem::exists("IASI_all"))
            std::filesystem::create_directory("IASI_ALL");
        for (int i = 0; i < 8461; i++)
        {
            std::cout << "Channel " << (i + 1) << std::endl;
            iasireader.getChannel(i).save_png(std::string("IASI_ALL/IASI-" + std::to_string(i + 1) + ".png").c_str());
        }
    }

    std::cout << "Channel IR imaging..." << std::endl;
    iasireader_img.getIRChannel().save_png(std::string("IASI-IMG.png").c_str());

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    int all_width_count = 150;
    int all_height_count = 60;
    cimg_library::CImg<unsigned short> imageAll(60 * all_width_count, iasireader.getChannel(0).height() * all_height_count, 1, 1);
    {
        int height = iasireader.getChannel(0).height();

        for (int row = 0; row < all_height_count; row++)
        {
            for (int column = 0; column < all_width_count; column++)
            {
                if (row * all_width_count + column >= 8461)
                    break;

                imageAll.draw_image(60 * column, height * row, 0, 0, iasireader.getChannel(row * all_width_count + column));
            }
        }
    }
    imageAll.save_png("IASI-ALL.png");

    data_in.close();
}
