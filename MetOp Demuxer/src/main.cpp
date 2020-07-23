#include <iostream>
#include <fstream>
#include <vector>

#include "deframer.h"
#include "reedsolomon.h"
#include "framer.h"

int main(int argc, char *argv[])
{
    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    // Read buffer
    uint8_t buffer[8192];

    CADUDeframer deframer;             // CADU Deframer
    CADUReedSolomon reedSolomon;       // RS correction
    CCSDSFramer framer(9, 103, 12966); // AVHRR data, frame length is fixed since anything else is corrupted data

    int rsfail = 0, ccsds = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, sizeof(uint8_t) * 8192);

        std::vector<std::array<uint8_t, CADU_SIZE>> correctedFrameBuffer; // RS Buffer for later use
        std::vector<std::vector<uint8_t>> ccsdsFrames;                    // ccsds buffer for later use

        // Push into deframer
        std::vector<std::array<uint8_t, CADU_SIZE>> frameBuffer = deframer.work(buffer, 8192);

        // Print status out
        if (deframer.getState() == 0)
            std::cout << "\rNOSYNC  " << std::flush;
        else if (deframer.getState() == 1)
            std::cout << "\rSYNCING " << std::flush;
        else if (deframer.getState() == 2)
            std::cout << "\rSYNCED  " << std::flush;

        // If we got frames in this batch, error-correct them! RS discard corrupted frames
        if (frameBuffer.size() > 0)
            correctedFrameBuffer = reedSolomon.work(frameBuffer, rsfail);

        // If RS didn't say this batch was garbage, (eg, frames remains), push it into the CCSDS framer
        if (correctedFrameBuffer.size() > 0)
            ccsdsFrames = framer.work(correctedFrameBuffer);

        // Count CCSDS frames
        ccsds += ccsdsFrames.size();

        // If we got frames, write them out
        if (ccsdsFrames.size() > 0)
            for (std::vector<uint8_t> &frame : ccsdsFrames)
            {
                //data_out.put(CADU_ASM_1);
                //data_out.put(CADU_ASM_2);
                //data_out.put(CADU_ASM_3);
                //data_out.put(CADU_ASM_4);
                for (uint8_t &byte : frame)
                    data_out.put(byte);
            }
    }

    std::cout << '\n';

    std::cout << "CADU Frames " << deframer.getFrameCount() << std::endl;
    std::cout << "RS Errors " << rsfail << std::endl;
    std::cout << "CCSDS Frames (VCID 3, APID 38) " << ccsds << std::endl;

    data_in.close();
    data_out.close();
}