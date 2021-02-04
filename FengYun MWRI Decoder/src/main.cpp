#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>
#include <cstring>
#include <ccsds/demuxer.h>
#include <ccsds/vcdu.h>
#include "erm_reader.h"

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

uint8_t reverseBits(uint8_t byte)
{
    byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return byte;
}

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
    ERMReader erm_reader;

    std::ofstream fileOut("data.bin");

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)&cadu, 1024);

        // Parse this transport frame
        libccsds::VCDU vcdu = libccsds::parseVCDU(cadu);

        //std::cout << "VC " << (int)vcdu.vcid << std::endl;

        // Right channel? (VCID 12 is MHS)
        if (vcdu.vcid == 12)
        {
            iasi_cadu++;

            // Demux
            std::vector<libccsds::CCSDSPacket> ccsdsFrames = ccsdsDemuxer.work(cadu);

            // Count frames
            ccsds += ccsdsFrames.size();

            // Push into processor (filtering APID 64)
            for (libccsds::CCSDSPacket &pkt : ccsdsFrames)
            {
                /*if (pkt.header.apid == 150)
                {
                    //std::cout << pkt.payload.size() << std::endl;
                    iasireader_img.work(pkt);
                    iasi_ccsds++;
                    //fileOut.write((char *)pkt.payload.data(), 6196);
                }
                else*/
                std::cout << "APID " << pkt.header.apid << std::endl;
                if (pkt.header.apid == 5)
                {

                    std::cout << "LEN " << pkt.payload.size() << std::endl;
                    //iasireader.work(pkt);
                    iasi_ccsds++;
                    erm_reader.work(pkt);

                    //for (int i = 0; i < pkt.payload.size(); i++)
                    //    pkt.payload[i] ^= 0xFF;

                    fileOut.write((char *)pkt.payload.data(), 1018);
                }
            }
        }

        // Show our progress
        // std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "VCID 12 (MHS) Frames : " << iasi_cadu << std::endl;
    std::cout << "CCSDS Frames         : " << ccsds << std::endl;
    std::cout << "MHS Frames           : " << iasi_ccsds << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    //for (int i = 0; i < 5; i++)
    //{
    //    std::cout << "Channel " << (i + 1) << std::endl;
    //    iasireader_img.getChannel(i).save_png(std::string("IASI-" + std::to_string(i + 1) + ".png").c_str());
    //}

    std::cout << "Channel IR imaging..." << std::endl;
    // iasireader_img.getIRChannel().save_png(std::string("IASI-IMG.png").c_str());

    erm_reader.getChannel(0).save_png(std::string("ERM-1.png").c_str());
    erm_reader.getChannel(1).save_png(std::string("ERM-2.png").c_str());

    data_in.close();
}
