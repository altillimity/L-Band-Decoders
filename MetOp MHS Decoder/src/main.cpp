#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "mhs_reader.h"

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
        std::cout << "Usage : " << argv[0] << " inputFrames.bin" << std::endl;
        return 0;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t mhs_cadu = 0, ccsds = 0, mhs_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << "  MHS Decoder by Aang23" << std::endl;
    std::cout << "           MetOp" << std::endl;
    std::cout << "      Fixed by Zbychu"       << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer(882, true);

    // Readers
    MHSReader mhsreader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 12 is MHS)
        if (vcdu.vcid == 12)
        {
            mhs_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 34)
                {
                    mhsreader.work(pkt);
                    mhs_ccsds++;
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 12 (MHS) Frames : " << mhs_cadu << std::endl;
    std::cout << "CCSDS Frames         : " << ccsds << std::endl;
    std::cout << "MHS Frames           : " << mhs_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    for (int i = 0; i < 5; i++)
    {
        std::cout << "Channel " << (i + 1) << std::endl;
        mhsreader.getChannel(i).save_png(std::string("MHS-" + std::to_string(i + 1) + ".png").c_str());
    }

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll(90 * 3, mhsreader.getChannel(0).height() * 2, 1, 1);
    {
        int height = mhsreader.getChannel(0).height();

        // Row 1
        imageAll.draw_image(90 * 0, 0, 0, 0, mhsreader.getChannel(0));
        imageAll.draw_image(90 * 1, 0, 0, 0, mhsreader.getChannel(1));
        imageAll.draw_image(90 * 2, 0, 0, 0, mhsreader.getChannel(2));

        // Row 2
        imageAll.draw_image(90 * 0, height, 0, 0, mhsreader.getChannel(3));
        imageAll.draw_image(90 * 1, height, 0, 0, mhsreader.getChannel(4));
    }
    imageAll.save_png("MHS-ALL.png");

    data_in.close();
}
