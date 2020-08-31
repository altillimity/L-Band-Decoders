#include <iostream>
#include <fstream>
#include <vector>

#include "tclap/CmdLine.h"

#include "deframer.h"
#include "manchester.h"
#include "simpledeframer.h"

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmd("METEOR Demuxer by Aang23", ' ', "1.0");

    // File arguments
    TCLAP::ValueArg<std::string> valueInput("i", "input", "Raw input frames", true, "", "frames.bin");
    TCLAP::ValueArg<std::string> valueOutput("o", "output", "Output filename for frames", true, "", "out");

    // Register all of the above options
    cmd.add(valueInput);
    cmd.add(valueOutput);

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
    std::ofstream data_out_msu_mr(valueOutput.getValue() + "-msu-mr.bin", std::ios::binary);
    std::ofstream data_out_tlm(valueOutput.getValue() + "-tlm.bin", std::ios::binary);
    std::ofstream data_out_bis_m(valueOutput.getValue() + "-bis-m.bin", std::ios::binary);
    std::ofstream data_out_mtvza(valueOutput.getValue() + "-mtvza.bin", std::ios::binary);
    std::ofstream data_out_sspd(valueOutput.getValue() + "-sspd.bin", std::ios::binary);

    // Read buffer
    uint8_t buffer[8192 * 2];
    uint8_t manchester_buffer[8192];

    CADUDeframer deframer; // CADU Deframer

    // MSU-MR data
    SimpleDeframer<uint64_t, 64, 11850 * 8, 0x0218A7A392DD9ABF> msumrDefra;
    // TLM data
    SimpleDeframer<uint64_t, 64, 74 * 8, 0x0218A7A392DD9ABF> tlmDefra;
    // BIS-M data
    SimpleDeframer<uint32_t, 32, 88 * 8, 0x71DE2CD8> bismDefra;
    // MTVZA data
    SimpleDeframer<uint64_t, 32, 248 * 8, 0xFB386A45> mtvzaDefra;
    // SSPD data
    SimpleDeframer<uint64_t, 24, 99 * 8, 0x42BB1F> sspdDefra;

    int msuMR_frames = 0, tlm_frames = 0, bisM_frames = 0, mtvza_frames = 0, sspd_frames = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, sizeof(uint8_t) * 8192 * 2);

        // Perform manchester decoding
        manchesterDecoder(buffer, 8192 * 2, manchester_buffer);

        // Push into deframer
        std::vector<std::array<uint8_t, CADU_SIZE>> frameBuffer = deframer.work(manchester_buffer, 8192);

        // Print status out
        if (deframer.getState() == 0)
            std::cout << "\rNOSYNC  " << std::flush;
        else if (deframer.getState() == 1)
            std::cout << "\rSYNCING " << std::flush;
        else if (deframer.getState() == 2)
            std::cout << "\rSYNCED  " << std::flush;

        std::vector<uint8_t> tlmData, bisMdata, sspdData, mtvzaData, msumrData;

        // Demux everything
        for (std::array<uint8_t, CADU_SIZE> &frame : frameBuffer)
        {
            // Extract TLM
            tlmData.insert(tlmData.end(), &frame[5 - 1], &frame[5 - 1] + 2);
            tlmData.insert(tlmData.end(), &frame[261 - 1], &frame[261 - 1] + 2);
            tlmData.insert(tlmData.end(), &frame[517 - 1], &frame[517 - 1] + 2);
            tlmData.insert(tlmData.end(), &frame[773 - 1], &frame[773 - 1] + 2);

            // Extract MSU-MR
            msumrData.insert(msumrData.end(), &frame[23 - 1], &frame[23 - 1] + 238);
            msumrData.insert(msumrData.end(), &frame[279 - 1], &frame[279 - 1] + 238);
            msumrData.insert(msumrData.end(), &frame[535 - 1], &frame[535 - 1] + 238);
            msumrData.insert(msumrData.end(), &frame[791 - 1], &frame[791 - 1] + 234);

            // Extract BIS-M
            bisMdata.insert(bisMdata.end(), &frame[7 - 1], &frame[7 - 1] + 4);
            bisMdata.insert(bisMdata.end(), &frame[263 - 1], &frame[263 - 1] + 4);
            bisMdata.insert(bisMdata.end(), &frame[519 - 1], &frame[519 - 1] + 4);
            bisMdata.insert(bisMdata.end(), &frame[775 - 1], &frame[775 - 1] + 4);

            // Extract MTVZA
            mtvzaData.insert(mtvzaData.end(), &frame[15 - 1], &frame[15 - 1] + 8);
            mtvzaData.insert(mtvzaData.end(), &frame[271 - 1], &frame[271 - 1] + 8);
            mtvzaData.insert(mtvzaData.end(), &frame[527 - 1], &frame[527 - 1] + 8);
            mtvzaData.insert(mtvzaData.end(), &frame[783 - 1], &frame[783 - 1] + 8);

            // Extract SSPD
            sspdData.insert(sspdData.end(), &frame[11 - 1], &frame[11 - 1] + 4);
            sspdData.insert(sspdData.end(), &frame[267 - 1], &frame[267 - 1] + 4);
            sspdData.insert(sspdData.end(), &frame[523 - 1], &frame[523 - 1] + 4);
            sspdData.insert(sspdData.end(), &frame[779 - 1], &frame[779 - 1] + 4);
        }

        // Deframe them all!
        std::vector<std::vector<uint8_t>> msumrFrames = msumrDefra.work(msumrData);
        std::vector<std::vector<uint8_t>> tlmFrames = tlmDefra.work(tlmData);
        std::vector<std::vector<uint8_t>> bismFrames = bismDefra.work(bisMdata);
        std::vector<std::vector<uint8_t>> mtvzaFrames = mtvzaDefra.work(mtvzaData);
        std::vector<std::vector<uint8_t>> sspdFrames = sspdDefra.work(sspdData);

        // Count them
        msuMR_frames += msumrFrames.size();
        tlm_frames += tlmFrames.size();
        bisM_frames += bismFrames.size();
        mtvza_frames += mtvzaFrames.size();
        sspd_frames += sspdFrames.size();

        // Write it out
        for (std::vector<uint8_t> &frame : msumrFrames)
            for (uint8_t &byte : frame)
                data_out_msu_mr.put(byte);

        for (std::vector<uint8_t> &frame : tlmFrames)
            for (uint8_t &byte : frame)
                data_out_tlm.put(byte);

        for (std::vector<uint8_t> &frame : bismFrames)
            for (uint8_t &byte : frame)
                data_out_bis_m.put(byte);

        for (std::vector<uint8_t> &frame : mtvzaFrames)
            for (uint8_t &byte : frame)
                data_out_mtvza.put(byte);

        for (std::vector<uint8_t> &frame : sspdFrames)
            for (uint8_t &byte : frame)
                data_out_sspd.put(byte);
    }

    std::cout << '\n';
    std::cout << "CADU Frames " << deframer.getFrameCount() << std::endl;
    std::cout << "MSU-MR Frames " << msuMR_frames << std::endl;
    std::cout << "TLM Frames " << tlm_frames << std::endl;
    std::cout << "BIS-M Frames " << bisM_frames << std::endl;
    std::cout << "MTVZA Frames " << mtvza_frames << std::endl;
    std::cout << "SSPD Frames " << sspd_frames << std::endl;
    //std::cout << "CCSDS Frames (VCID " << valueVcid.getValue() << ", APID " << valueApid.getValue() << ") " << ccsds << std::endl;

    data_in.close();
    data_out_msu_mr.close();
    data_out_tlm.close();
    data_out_bis_m.close();
    data_out_mtvza.close();
    data_out_sspd.close();
}
