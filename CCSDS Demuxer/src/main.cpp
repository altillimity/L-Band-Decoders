#include <iostream>
#include <fstream>
#include <vector>

#include "tclap/CmdLine.h"

#include "deframer.h"
#include "reedsolomon.h"
#include "framer.h"
#include "simpledeframer.h"

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmd("CCSDS Demuxer by Aang23", ' ', "1.0");

    // File arguments
    TCLAP::ValueArg<std::string> valueInput("i", "input", "Raw input frames", true, "", "frames.bin");
    TCLAP::ValueArg<std::string> valueOutput("o", "output", "Output CCSDS frames", true, "", "out.bin");

    // VCID / APID to extract
    TCLAP::ValueArg<int> valueVcid("v", "vcid", "Virtual Channel ID", true, 0, "vcid");
    TCLAP::ValueArg<int> valueApid("a", "apid", "APID", false, -1, "apid");
    TCLAP::ValueArg<int> valueSize("s", "size", "Frame size", true, 0, "size");
    TCLAP::SwitchArg valueFengYun("f", "fengyun", "FengYun imager deframing");
    TCLAP::SwitchArg valueAddHeader("m", "marker", "Add sync marker (1ACFFC1D) for easy syncing");
    TCLAP::SwitchArg valueFrameLength("l", "framelength", "Show found frame length");

    // Register all of the above options
    cmd.add(valueInput);
    cmd.add(valueOutput);
    cmd.add(valueVcid);
    cmd.add(valueApid);
    cmd.add(valueSize);
    cmd.add(valueFengYun);
    cmd.add(valueAddHeader);
    cmd.add(valueFrameLength);

    // Parse
    try
    {
        cmd.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e)
    {
        std::cout << e.error() << '\n';
        return 0;
    }

    // Output and Input file
    std::ifstream data_in(valueInput.getValue(), std::ios::binary);
    std::ofstream data_out(valueOutput.getValue(), std::ios::binary);

    // Read buffer
    uint8_t buffer[8192];

    CADUDeframer deframer;                                                                // CADU Deframer
    CADUReedSolomon reedSolomon;                                                          // RS correction
    CCSDSFramer framer(valueVcid.getValue(), valueApid.getValue(), valueSize.getValue()); // CCSDS Framer

    SimpleDeframer<uint64_t, 60, 208400, 0b101000010001011011111101011100011001110110000011110010010101> fengyunFramer;

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
        if (!valueFengYun.getValue())
            ccsds += ccsdsFrames.size();

        // If we got frames, write them out
        if (ccsdsFrames.size() > 0)
        {
            std::vector<std::vector<uint8_t>> dataFrames;
            // Frame FY frames and count them
            if (valueFengYun.getValue())
            {
                dataFrames = fengyunFramer.work(ccsdsFrames);
                ccsds += dataFrames.size();
            }

            // Write to file
            if ((valueFengYun.getValue() ? dataFrames : ccsdsFrames).size() > 0)
                for (std::vector<uint8_t> &frame : (valueFengYun.getValue() ? dataFrames : ccsdsFrames))
                {
                    // Write header
                    if (valueAddHeader.getValue())
                    {
                        data_out.put(CADU_ASM_1);
                        data_out.put(CADU_ASM_2);
                        data_out.put(CADU_ASM_3);
                        data_out.put(CADU_ASM_4);
                    }

                    if (valueFrameLength.getValue())
                        std::cout << frame.size() << std::endl;

                    for (uint8_t &byte : frame)
                        data_out.put(byte);
                }
        }
    }

    std::cout << '\n';
    std::cout << "CADU Frames " << deframer.getFrameCount() << std::endl;
    std::cout << "RS Errors " << rsfail << std::endl;
    std::cout << "CCSDS Frames (VCID " << valueVcid.getValue() << ", APID " << valueApid.getValue() << ") " << ccsds << std::endl;

    data_in.close();
    data_out.close();
}
