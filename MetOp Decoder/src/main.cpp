#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "deframer.h"
#include "reedsolomon.h"
#include "viterbi.h"

#ifndef _WIN32
#include <unistd.h>
#else
#include "getopt/getopt.h"
#endif
// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// Processing buffer size
#define BUFFER_SIZE (8738 * 5)

int main(int argc, char *argv[])
{
    // Print out command syntax
    if (argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " -v 0.165 -o 5 inputfile.raw outputframes.bin" << std::endl;
        std::cout << "		    -o (outsinc after decode frame number(default: 5))" << std::endl;
        std::cout << "		    -v (viterbi treshold(default: 0.170))" << std::endl;
        std::cout << "		    -h (enable hard symbols input (slower))" << std::endl;
        std::cout << "2020-08-15." << std::endl;
        return 1;
    }

    // Variables
    int viterbi_outsync_after = 5;
    float viterbi_ber_threasold = 0.170;
    int sw = 0;
    bool softSymbols = true;

    while ((sw = getopt(argc, argv, "oh:v:")) != -1)
    {
        switch (sw)
        {
        case 'o':
            viterbi_outsync_after = std::atof(optarg);
            break;
        case 'v':
            viterbi_ber_threasold = std::atof(optarg);
            break;
        case 'h':
            softSymbols = false;
            break;
        default:
            break;
        }
    }

    // Output and Input file
    std::ifstream data_in(argv[argc - 2], std::ios::binary);
    std::ofstream data_out(argv[argc - 1], std::ios::binary);

    // MetOp Viterbi decoder
    MetopViterbi viterbi(true, viterbi_ber_threasold, 1, viterbi_outsync_after, 50);

    SatHelper::ReedSolomon reedSolomon;
    CADUDeframer deframer;

    // Viterbi output buffer
    uint8_t *viterbi_out = new uint8_t[BUFFER_SIZE];

    // Read buffer
    std::complex<float> buffer[BUFFER_SIZE];
    int8_t *soft_buffer = new int8_t[BUFFER_SIZE * 2];

    // Complete filesize
    size_t filesize = getFilesize(argv[argc - 2]);

    // Data we wrote out
    size_t data_out_total = 0;

    // Print infos and credits out
    std::cout << "---------------------------" << std::endl;
    std::cout << "MetOp Decoder by Aang23" << std::endl;
    std::cout << "Fixed by Tomi HA6NAB" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Viterbi threshold: " << viterbi_ber_threasold << std::endl;
    std::cout << "Outsinc after: " << viterbi_outsync_after << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // Work buffers
    uint8_t rsWorkBuffer[255];

    // Read until EOF
    while (!data_in.eof())
    {
        // Read a buffer
        if (softSymbols)
        {
            data_in.read((char *)soft_buffer, BUFFER_SIZE * 2);

            // Convert to hard symbols from soft symbols. We may want to work with soft only later?
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                using namespace std::complex_literals;
                buffer[i] = ((float)soft_buffer[i * 2 + 1] / 127.0f) + ((float)soft_buffer[i * 2] / 127.0f) * 1if;
            }
        }
        else
        {
            data_in.read((char *)buffer, sizeof(std::complex<float>) * BUFFER_SIZE);
        }

        // Push into Viterbi
        int num_samp = viterbi.work(buffer, BUFFER_SIZE, viterbi_out);

        // Reconstruct into bytes and write to output file
        if (num_samp > 0)
        {
            // Deframe / derand
            std::vector<std::array<uint8_t, CADU_SIZE>> frames = deframer.work(viterbi_out, num_samp);

            if (frames.size() > 0)
            {
                for (std::array<uint8_t, CADU_SIZE> cadu : frames)
                {
                    // RS Decoding
                    int errors = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        reedSolomon.deinterleave(&cadu[4], rsWorkBuffer, i, 4);
                        errors = reedSolomon.decode_ccsds(rsWorkBuffer);
                        reedSolomon.interleave(rsWorkBuffer, &cadu[4], i, 4);
                    }

                    // Write it out
                    data_out_total += CADU_SIZE;
                    data_out.write((char *)&cadu, CADU_SIZE);
                }
            }
        }

        // Console stuff
        std::cout << '\r' << "Viterbi : " << (viterbi.getState() == 0 ? "NO SYNC" : viterbi.getState() == 1 ? "SYNCING"
                                                                                                            : "SYNCED");
        if (deframer.getState() == 0)
            std::cout << ", Deframer : NOSYNC" << std::flush;
        else if (deframer.getState() == 2 | deframer.getState() == 6)
            std::cout << ", Deframer : SYNCING" << std::flush;
        else if (deframer.getState() > 6)
            std::cout << ", Deframer : SYNCED" << std::flush;
        std::cout << ", CADUs : " << (float)(data_out_total / 1024) << ", Data out : " << round(data_out_total / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl
              << "Done! Enjoy" << std::endl;

    data_in.close();
    data_out.close();
}
