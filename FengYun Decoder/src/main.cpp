#ifndef _WIN32
#include <unistd.h>
#else
#include "getopt/getopt.h"
#endif

#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <thread>

#include "viterbi.h"
#include "diff.h"

// Processing buffer size
#define BUFFER_SIZE (8192 * 5)

// Small function that returns 1 bit from any type
template <typename T>
inline bool getBit(T data, int bit)
{
    return (data >> bit) & 1;
}

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

int main(int argc, char *argv[])
{
    // Print out command syntax
    if (argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " -b -v 0.165 -o 5 inputfile.bin outputframes.bin" << std::endl;
        std::cout << "		    -b (decode the FY3A,B sat.)" << std::endl;
        std::cout << "		    -c (decode the FY3C,D? sat.)" << std::endl;
        std::cout << "		    -v (viterbi treshold(default: 0.170))" << std::endl;
        std::cout << "		    -o (outsinc after decode frame number(default: 5))" << std::endl;
        std::cout << "2020-08-15." << std::endl;
        return 1;
    }

    // Variables
    int viterbi_outsync_after = 5;
    float viterbi_ber_threasold = 0.170;
    int fy3c_mode = 0;
    int sw = 0;

    while ((sw = getopt(argc, argv, "bco:v:")) != -1)
    {
        switch (sw)
        {
        case 'b':
            fy3c_mode = 0;
            break;
        case 'c':
            fy3c_mode = 1;
            break;
        case 'o':
            viterbi_outsync_after = std::atof(optarg);
            break;
        case 'v':
            viterbi_ber_threasold = std::atof(optarg);
            break;
        default:
            break;
        }
    }

    // Output and Input file
    std::ifstream data_in(argv[argc - 2], std::ios::binary);
    std::ofstream data_out(argv[argc - 1], std::ios::binary);

    // Our 2 Viterbi decoders and differential decoder
    FengyunViterbi viterbi1(true, viterbi_ber_threasold, 1, viterbi_outsync_after, 50), viterbi2(true, viterbi_ber_threasold, 1, viterbi_outsync_after, 50);
    FengyunDiff diff;

    // Viterbi output buffer
    uint8_t *viterbi1_out = new uint8_t[BUFFER_SIZE];
    uint8_t *viterbi2_out = new uint8_t[BUFFER_SIZE];

    // A few vectors for processing
    std::vector<std::complex<float>> *iSamples = new std::vector<std::complex<float>>(BUFFER_SIZE),
                                     *qSamples = new std::vector<std::complex<float>>(BUFFER_SIZE);

    // Read buffer
    std::complex<float> buffer[BUFFER_SIZE];

    // Diff decoder input and output
    std::vector<uint8_t> *diff_in = new std::vector<uint8_t>, *diff_out = new std::vector<uint8_t>;

    // Complete filesize
    size_t filesize = getFilesize(argv[argc - 2]);

    // Data we wrote out
    size_t data_out_total = 0;

    std::cout << "---------------------------" << std::endl;
    std::cout << "FengYun Decoder by Aang23" << std::endl;
    std::cout << "Fixed by Tomi HA6NAB" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Viterbi threshold: " << viterbi_ber_threasold << std::endl;
    std::cout << "Outsinc after: " << viterbi_outsync_after << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    int shift = 0;

    // Read until there is no more data
    while (!data_in.eof())
    {

        // Read a buffer
        data_in.read((char *)buffer, sizeof(std::complex<float>) * BUFFER_SIZE);

        // Deinterleave I & Q for the 2 Viterbis
        for (int i = 0; i < BUFFER_SIZE / 2; i++)
        {
            using namespace std::complex_literals;
            std::complex<float> iS = buffer[i * 2 + shift].imag() + buffer[i * 2 + 1 + shift].imag() * 1if;
            std::complex<float> qS = buffer[i * 2 + shift].real() + buffer[i * 2 + 1 + shift].real() * 1if;
            iSamples->push_back(iS);
            if (fy3c_mode)
            {
                qSamples->push_back(-qS); //FY3C
            }
            else
            {
                qSamples->push_back(qS); // FY3B
            }
        }
        // Run Viterbi!
        int v1 = viterbi1.work(*qSamples, qSamples->size(), viterbi1_out);
        int v2 = viterbi2.work(*iSamples, iSamples->size(), viterbi2_out);

        // Interleave and pack output into 2 bits chunks
        if (v1 > 0 || v2 > 0)
        {
            if (v1 == v2 && v1 > 0)
            {
                uint8_t bit1, bit2, bitCb;
                for (int y = 0; y < v1; y++)
                {
                    for (int i = 7; i >= 0; i--)
                    {
                        bit1 = getBit<uint8_t>(viterbi1_out[y], i);
                        bit2 = getBit<uint8_t>(viterbi2_out[y], i);
                        bitCb = bit2 << 1 | bit1;
                        diff_in->push_back(bitCb);
                    }
                }
            }
        }
        else
        {
            if (shift)
            {
                shift = 0;
            }
            else
            {
                shift = 1;
            }
            diff_in->clear();
            iSamples->clear();
            qSamples->clear();
            // Deinterleave I & Q for the 2 Viterbis
            for (int i = 0; i < BUFFER_SIZE / 2; i++)
            {
                using namespace std::complex_literals;
                std::complex<float> iS = buffer[i * 2 + shift].imag() + buffer[i * 2 + 1 + shift].imag() * 1if;
                std::complex<float> qS = buffer[i * 2 + shift].real() + buffer[i * 2 + 1 + shift].real() * 1if;
                iSamples->push_back(iS);
                if (fy3c_mode)
                {
                    qSamples->push_back(-qS); //FY3C
                }
                else
                {
                    qSamples->push_back(qS); // FY3B
                }
            }
            // Run Viterbi!
            int v1 = viterbi1.work(*qSamples, qSamples->size(), viterbi1_out);
            int v2 = viterbi2.work(*iSamples, iSamples->size(), viterbi2_out);

            // Interleave and pack output into 2 bits chunks
            if (v1 > 0 || v2 > 0)
            {
                if (v1 == v2 && v1 > 0)
                {
                    uint8_t bit1, bit2, bitCb;
                    for (int y = 0; y < v1; y++)
                    {
                        for (int i = 7; i >= 0; i--)
                        {
                            bit1 = getBit<uint8_t>(viterbi1_out[y], i);
                            bit2 = getBit<uint8_t>(viterbi2_out[y], i);
                            bitCb = bit2 << 1 | bit1;
                            diff_in->push_back(bitCb);
                        }
                    }
                }
            }
            else
            {
                if (shift)
                {
                    shift = 0;
                }
                else
                {
                    shift = 1;
                }
            }
        }

        // Perform differential decoding
        *diff_out = diff.work(*diff_in);

        // Reconstruct into bytes and write to output file
        for (int i = 0; i < diff_out->size() / 4; i++)
        {
            uint8_t toPush = ((*diff_out)[i * 4] << 6) | ((*diff_out)[i * 4 + 1] << 4) | ((*diff_out)[i * 4 + 2] << 2) | (*diff_out)[i * 4 + 3];
            data_out.write((char *)&toPush, 1);
        }

        data_out_total += diff_out->size() / 4;

        // Console stuff
        std::cout << '\r' << "Viterbi 1 : " << (viterbi1.getState() == 0 ? "NO SYNC" : viterbi1.getState() == 1 ? "SYNCING" : "SYNCED") << ", Viterbi 2 : " << (viterbi2.getState() == 0 ? "NO SYNC" : viterbi2.getState() == 1 ? "SYNCING" : "SYNCED") << ", Data out : " << round(data_out_total / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;

        // Clear everything for the next run
        diff_in->clear();
        iSamples->clear();
        qSamples->clear();
    }

    std::cout << std::endl
              << "Done! Enjoy" << std::endl;

    // Close files
    data_in.close();
    data_out.close();
}
