#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <thread>

#include "viterbi.h"
#include "diff.h"

#define BUFFER_SIZE (8192 * 5)

template <typename T>
inline bool getBit(T data, int bit)
{
    return (data >> bit) & 1;
}

int main(int argc, char *argv[])
{
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    FengyunViterbi viterbi1(true, 0.150f, 5, 20, 50), viterbi2(true, 0.150f, 5, 20, 50);
    FengyunDiff diff;

    uint8_t *viterbi1_out = new uint8_t[BUFFER_SIZE];
    uint8_t *viterbi2_out = new uint8_t[BUFFER_SIZE];

    std::vector<std::complex<float>> *samples = new std::vector<std::complex<float>>(BUFFER_SIZE),
                                     *samples2 = new std::vector<std::complex<float>>(BUFFER_SIZE),
                                     *iSamples = new std::vector<std::complex<float>>(BUFFER_SIZE),
                                     *qSamples = new std::vector<std::complex<float>>(BUFFER_SIZE);

    std::complex<float> buffer[BUFFER_SIZE];

    std::vector<uint8_t> *diff_in = new std::vector<uint8_t>, *diff_out = new std::vector<uint8_t>;

    while (!data_in.eof())
    {
        data_in.read((char *)buffer, sizeof(std::complex<float>) * BUFFER_SIZE);

        for (int i = 0; i < BUFFER_SIZE / 2; i++)
        {
            using namespace std::complex_literals;
            std::complex<float> iS = buffer[i * 2].imag() + buffer[i * 2 + 1].imag() * 1if;
            std::complex<float> qS = buffer[i * 2].real() + buffer[i * 2 + 1].real() * 1if;
            iSamples->push_back(iS);
            qSamples->push_back(qS);
        }

        int v1 = viterbi1.work(*qSamples, qSamples->size(), viterbi1_out);
        int v2 = viterbi2.work(*iSamples, iSamples->size(), viterbi2_out);

        if (v1 == v2 && v1 > 0 && v2 > 0)
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

        *diff_out = diff.work(*diff_in);

        for (int i = 0; i < diff_out->size() / 4; i++)
        {
            uint8_t toPush = ((*diff_out)[i * 4] << 6) | ((*diff_out)[i * 4 + 1] << 4) | ((*diff_out)[i * 4 + 2] << 2) | (*diff_out)[i * 4 + 3];
            data_out.write((char *)&toPush, 1);
        }

        diff_in->clear();
        iSamples->clear();
        qSamples->clear();
    }

    data_in.close();
    data_out.close();
}