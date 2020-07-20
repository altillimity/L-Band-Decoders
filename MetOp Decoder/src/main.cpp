#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "viterbi.h"

// Processing buffer size
#define BUFFER_SIZE (8192 * 5)

int main(int argc, char *argv[])
{
    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    // MetOp Viterbi decoder
    MetopViterbi viterbi(true, 0.150f, 5, 20, 50);

    // Viterbi output buffer
    uint8_t *viterbi_out = new uint8_t[BUFFER_SIZE];

    // Read buffer
    std::complex<float> buffer[BUFFER_SIZE];

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, sizeof(std::complex<float>) * BUFFER_SIZE);

        // Push into Viterbi
        int num_samp = viterbi.work(buffer, BUFFER_SIZE, viterbi_out);

        // Write output
        if (num_samp > 0)
            data_out.write((char *)viterbi_out, num_samp);
    }

    data_in.close();
    data_out.close();
}