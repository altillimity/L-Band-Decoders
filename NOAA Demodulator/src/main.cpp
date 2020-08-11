#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cstdint>

#include "agc.h"
#include "pll.h"
#include "moving_average.h"
#include "clock_recovery.h"
#include "deframer.h"
#include "pp_buffer.h"

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

void volk_32f_binary_slicer_8i_generic(int8_t *cVector, const float *aVector, unsigned int num_points)
{
    int8_t *cPtr = cVector;
    const float *aPtr = aVector;
    unsigned int number = 0;

    for (number = 0; number < num_points; number++)
    {
        if (*aPtr++ >= 0)
        {
            *cPtr++ = 1;
        }
        else
        {
            *cPtr++ = 0;
        }
    }
}

#define BUFFER_SIZE 8192 * 10
#define SAMPLE_RATE (float)6e6
#define SYMBOL_RATE (float)(600 * 1109)
#define SPS (SAMPLE_RATE / SYMBOL_RATE)

int main(int argc, char *argv[])
{
    std::ifstream data_in(argv[1], std::ios::binary);
    std::ofstream data_out(argv[2], std::ios::binary);

    size_t filesize = getFilesize(argv[1]);

    std::complex<float> buffer[BUFFER_SIZE];
    std::complex<float> agc_buff[BUFFER_SIZE];
    float pll_buff[BUFFER_SIZE];
    float moving_buff[BUFFER_SIZE];
    float recovered_buff[BUFFER_SIZE];
    uint8_t bitsBuffer[BUFFER_SIZE];
    std::vector<std::complex<float>> clockRecoIn;
    std::vector<std::complex<float>> clockRecoOut;
    std::vector<uint16_t> frames;

    int framec = 0;

    Agc agc = Agc(AGC_WINSIZE, 0.5 / 32768.0, 1.0);
    PLL pll = PLL(0.01f, pow(0.01, 2) / 4.0, 3.0f * M_PI * 100e3f / SAMPLE_RATE);
    MovingAverage movingAverage = MovingAverage(round(SPS / 4.0f), 1.0f / 4.0, BUFFER_SIZE, 1);
    ClockRecovery clockReco = ClockRecovery(SPS / 2.0f, pow(0.01, 2) / 4.0, 0.5f, 0.01f, 100e-6f);
    NOAADeframer deframer;

    while (!data_in.eof())
    {
        data_in.read((char *)buffer, BUFFER_SIZE * sizeof(std::complex<float>));

        agc.work(buffer, BUFFER_SIZE, agc_buff);
        pll.work(agc_buff, BUFFER_SIZE, pll_buff);

        int numMovingAvg = movingAverage.work(pll_buff, BUFFER_SIZE, moving_buff);

        for (int i = 0; i < numMovingAvg; i++)
            clockRecoIn.push_back(std::complex<float>(moving_buff[i], 0));
        clockRecoOut = clockReco.work(clockRecoIn, clockRecoIn.size());
        int recovered_size = clockRecoOut.size();
        for (int i = 0; i < recovered_size; i++)
            recovered_buff[i] = clockRecoOut[i].real();

        if (recovered_size > 0)
        {
            volk_32f_binary_slicer_8i_generic((int8_t *)bitsBuffer, recovered_buff, recovered_size);
            frames = deframer.work(bitsBuffer, recovered_size);
        }

        framec += frames.size();

        if (frames.size() > 0)
            for (uint16_t &frame : frames)
                data_out.write((char *)&frame, sizeof(uint16_t));

        // Console stuff
        std::cout << '\r' << "Frames : " << framec / 11090 << ", Data out : " << round(framec * 2 / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;

        frames.clear();
        clockRecoOut.clear();
        clockRecoIn.clear();
    }

    data_in.close();
    data_out.close();
}