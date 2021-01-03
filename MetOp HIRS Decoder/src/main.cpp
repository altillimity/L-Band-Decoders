#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "hirs_reader.h"

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
    std::cout << "  HIRS Decoder by Aang23" << std::endl;
    std::cout << "         MetOp" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer(882, true);

    // Readers
    HIRSReader hirsreader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 3 is HIRS)
        if (vcdu.vcid == 3)
        {
            mhs_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 38)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 38)
                {
                    hirsreader.work(pkt);
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

    for (int i = 0; i < 20; i++)
    {
        std::cout << "Channel " << (i + 1) << std::endl;
        hirsreader.getChannel(i).save_png(std::string("HIRS-" + std::to_string(i + 1) + ".png").c_str());
    }

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll(56 * 5, hirsreader.getChannel(0).height() * 4, 1, 1);
    {
        int height = hirsreader.getChannel(0).height();

        // Row 1
        imageAll.draw_image(56 * 0, 0, 0, 0, hirsreader.getChannel(0));
        imageAll.draw_image(56 * 1, 0, 0, 0, hirsreader.getChannel(1));
        imageAll.draw_image(56 * 2, 0, 0, 0, hirsreader.getChannel(2));
        imageAll.draw_image(56 * 3, 0, 0, 0, hirsreader.getChannel(3));
        imageAll.draw_image(56 * 4, 0, 0, 0, hirsreader.getChannel(4));

        // Row 2
        imageAll.draw_image(56 * 0, height, 0, 0, hirsreader.getChannel(5));
        imageAll.draw_image(56 * 1, height, 0, 0, hirsreader.getChannel(6));
        imageAll.draw_image(56 * 2, height, 0, 0, hirsreader.getChannel(7));
        imageAll.draw_image(56 * 3, height, 0, 0, hirsreader.getChannel(8));
        imageAll.draw_image(56 * 4, height, 0, 0, hirsreader.getChannel(9));

        // Row 3
        imageAll.draw_image(56 * 0, height * 2, 0, 0, hirsreader.getChannel(10));
        imageAll.draw_image(56 * 1, height * 2, 0, 0, hirsreader.getChannel(11));
        imageAll.draw_image(56 * 2, height * 2, 0, 0, hirsreader.getChannel(12));
        imageAll.draw_image(56 * 3, height * 2, 0, 0, hirsreader.getChannel(13));
        imageAll.draw_image(56 * 4, height * 2, 0, 0, hirsreader.getChannel(14));

        // Row 4
        imageAll.draw_image(56 * 0, height * 3, 0, 0, hirsreader.getChannel(15));
        imageAll.draw_image(56 * 1, height * 3, 0, 0, hirsreader.getChannel(16));
        imageAll.draw_image(56 * 2, height * 3, 0, 0, hirsreader.getChannel(17));
        imageAll.draw_image(56 * 3, height * 3, 0, 0, hirsreader.getChannel(18));
        imageAll.draw_image(56 * 4, height * 3, 0, 0, hirsreader.getChannel(19));
    }
    imageAll.resize(imageAll.width() * 10, imageAll.height() * 10);
    imageAll.save_png("HIRS-ALL.png");

    data_in.close();
}
