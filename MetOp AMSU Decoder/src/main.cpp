#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "amsu_a1_reader.h"
#include "amsu_a2_reader.h"

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
    uint64_t amsu_cadu = 0, ccsds = 0, amsu1_ccsds = 0, amsu2_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " AMSU Decoder by Aang23" << std::endl;
    std::cout << "         MetOp" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer = libccsds::Demuxer(882, true);

    // Readers
    AMSUA1Reader a1reader;
    AMSUA2Reader a2reader;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 3 is AMSU)
        if (vcdu.vcid == 3)
        {
            amsu_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 39)
                {
                    a1reader.work(pkt);
                    amsu1_ccsds++;
                }
                if (pkt.header.apid == 40)
                {
                    a2reader.work(pkt);
                    amsu2_ccsds++;
                }
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 3 (AMSU) Frames : " << amsu_cadu << std::endl;
    std::cout << "CCSDS Frames         : " << ccsds << std::endl;
    std::cout << "AMSU A1 Frames       : " << amsu1_ccsds << std::endl;
    std::cout << "AMSU A2 Frames       : " << amsu2_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    for (int i = 0; i < 2; i++)
    {
        std::cout << "Channel " << (i + 1) << std::endl;
        a2reader.getChannel(i).save_png(std::string("AMSU-A2-" + std::to_string(i + 1) + ".png").c_str());
    }

    for (int i = 0; i < 13; i++)
    {
        std::cout << "Channel " << (i + 3) << std::endl;
        a1reader.getChannel(i).save_png(std::string("AMSU-A1-" + std::to_string(i + 3) + ".png").c_str());
    }

    // Output a few nice composites as well
    std::cout << "Global Composite..." << std::endl;
    cimg_library::CImg<unsigned short> imageAll(30 * 8, a1reader.getChannel(0).height() * 2, 1, 1);
    {
        int height = a1reader.getChannel(0).height();

        // Row 1
        imageAll.draw_image(30 * 0, 0, 0, 0, a2reader.getChannel(0));
        imageAll.draw_image(30 * 1, 0, 0, 0, a2reader.getChannel(1));
        imageAll.draw_image(30 * 2, 0, 0, 0, a1reader.getChannel(0));
        imageAll.draw_image(30 * 3, 0, 0, 0, a1reader.getChannel(1));
        imageAll.draw_image(30 * 4, 0, 0, 0, a1reader.getChannel(2));
        imageAll.draw_image(30 * 5, 0, 0, 0, a1reader.getChannel(3));
        imageAll.draw_image(30 * 6, 0, 0, 0, a1reader.getChannel(4));
        imageAll.draw_image(30 * 7, 0, 0, 0, a1reader.getChannel(5));

        // Row 2
        imageAll.draw_image(30 * 0, height, 0, 0, a1reader.getChannel(6));
        imageAll.draw_image(30 * 1, height, 0, 0, a1reader.getChannel(7));
        imageAll.draw_image(30 * 2, height, 0, 0, a1reader.getChannel(8));
        imageAll.draw_image(30 * 3, height, 0, 0, a1reader.getChannel(9));
        imageAll.draw_image(30 * 4, height, 0, 0, a1reader.getChannel(10));
        imageAll.draw_image(30 * 5, height, 0, 0, a1reader.getChannel(11));
        imageAll.draw_image(30 * 6, height, 0, 0, a1reader.getChannel(12));
    }
    imageAll.save_png("AMSU-ALL.png");

    data_in.close();
}
