#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>

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
std::ofstream data_out;

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        std::cout << "Usage (default Aqua, -t for Terra): " << argv[0] << " inputFrames.bin" << std::endl;
        return 0;
    }

    bool terra = false;

    // Terra mode?
    if (argc == 3)
    {
        if (std::string(argv[2]) == "-t")
            terra = true;
    }

    // Complete filesize
    size_t filesize = getFilesize(argv[1]);

    // Output and Input file
    data_in = std::ifstream(argv[1], std::ios::binary);

    // Read buffer
    libccsds::CADU cadu;

    // Counters
    uint64_t modis_cadu = 0, ccsds = 0, modis_ccsds = 0;

    // Graphics
    std::cout << "---------------------------" << std::endl;
    std::cout << " MODIS Decoder by Aang23" << std::endl;
    std::cout << "      Aqua / Terra" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    libccsds::Demuxer ccsdsDemuxer;

    data_out = std::ofstream("test.bin", std::ios::binary);

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        // Right channel? (VCID 30/42 is MODIS)
        if (vcdu.vcid == 10)
        {
            modis_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                if (pkt.header.apid == 150 && pkt.payload.size() == 6196)
                {
                    modis_ccsds++;
                    data_out.write((char *)&pkt.payload[0], 6196);
                    int scan_pos = pkt.payload[10];
                    std::cout << scan_pos << std::endl;
                }
            }
        }

        // Show our progress
        //std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 30 (MODIS) Frames : " << modis_cadu << std::endl;
    std::cout << "CCSDS Frames           : " << ccsds << std::endl;
    std::cout << "MODIS CCSDS Frames     : " << modis_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    data_in.close();
    data_out.close();
}
