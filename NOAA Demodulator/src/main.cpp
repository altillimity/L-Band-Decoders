#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cstdint>
#include "tclap/CmdLine.h"
#include "agc.h"
#include "pll.h"
#include "moving_average.h"
#include "clock_recovery.h"
#include "deframer.h"

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
#define SYMBOL_RATE (float)(600 * 1109)

/* TODO : THIS WILL NEED A HUGE CLEANUP */

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmd("NOAA HRPT Demodulator / Deframer by Aang23", ' ', "1.0");

    // File arguments
    TCLAP::ValueArg<std::string> valueInput("i", "input", "Baseband input", true, "", "baseband.raw");
    TCLAP::ValueArg<std::string> valueOutput("o", "output", "Output frames", true, "", "frames.bin");

    // VCID / APID to extract
    TCLAP::ValueArg<int> valueSamplerate("s", "samplerate", "Baseband samplerate", true, 6000000, "6000000");

    // Baseband format
    TCLAP::SwitchArg valueF32Baseband("f", "float32", "Input baseband as float32");
    TCLAP::SwitchArg valueInt16Baseband("6", "int16", "Input baseband as int16");
    TCLAP::SwitchArg valueInt8Baseband("8", "int8", "Input baseband as int8");

    // Register all of the above options
    cmd.add(valueInput);
    cmd.add(valueOutput);
    cmd.add(valueSamplerate);
    cmd.add(valueF32Baseband);
    cmd.add(valueInt16Baseband);
    cmd.add(valueInt8Baseband);

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

    // Just in case
    if (!(valueF32Baseband.getValue() || valueInt16Baseband.getValue() || valueInt8Baseband.getValue()))
    {
        std::cout << "Please specify baseband format! -f, -6, -8" << std::endl;
        return 1;
    }
    else if ((valueF32Baseband.getValue() + valueInt16Baseband.getValue() + valueInt8Baseband.getValue()) > 1)
    {
        std::cout << "Please specify only one baseband format! -f, -6, -8" << std::endl;
        return 1;
    }

    // Filezise and progress stuff
    size_t filesize = getFilesize(valueInput.getValue());
    size_t dataout = 0;

    // Input / output files
    std::ifstream data_in(valueInput.getValue(), std::ios::binary);
    std::ofstream data_out(valueOutput.getValue(), std::ios::binary);

    // Buffer mess
    std::complex<float> buffer[BUFFER_SIZE];
    std::complex<float> agc_buff[BUFFER_SIZE];
    float pll_buff[BUFFER_SIZE];
    float moving_buff[BUFFER_SIZE];
    float recovered_buff[BUFFER_SIZE];
    uint8_t bitsBuffer[BUFFER_SIZE];
    std::vector<std::complex<float>> clockRecoIn;
    std::vector<std::complex<float>> clockRecoOut;
    std::vector<uint16_t> frames;

    int frame_count = 0;

    Agc agc = Agc(AGC_WINSIZE, 0.5 / 32768.0, 1.0);
    PLL pll = PLL(0.01f, pow(0.01, 2) / 4.0, 3.0f * M_PI * 100e3f / (float) valueSamplerate.getValue());
    MovingAverage movingAverage = MovingAverage(round(((float) valueSamplerate.getValue() / SYMBOL_RATE) / 4.0f), 1.0f / 4.0, BUFFER_SIZE, 1);
    ClockRecovery clockReco = ClockRecovery(((float) valueSamplerate.getValue() / SYMBOL_RATE) / 2.0f, pow(0.01, 2) / 4.0, 0.5f, 0.01f, 100e-6f);
    NOAADeframer deframer;

    // Int16 buffer
    int16_t buffer_i16[BUFFER_SIZE * 2];

    // Int8 buffer
    int8_t buffer_i8[BUFFER_SIZE * 2];

    while (!data_in.eof())
    {
        // Get baseband, possibly convert to F32
        if (valueF32Baseband.getValue())
        {
            data_in.read((char *)buffer, BUFFER_SIZE * sizeof(std::complex<float>));
        }
        else if (valueInt16Baseband.getValue())
        {
            data_in.read((char *)buffer_i16, BUFFER_SIZE * sizeof(int16_t) * 2);
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                using namespace std::complex_literals;
                buffer[i] = (float)buffer_i16[i * 2] + (float)buffer_i16[i * 2 + 1] * 1if;
            }
        }
        else if (valueInt8Baseband.getValue())
        {
            data_in.read((char *)buffer_i8, BUFFER_SIZE * sizeof(int8_t) * 2);
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                using namespace std::complex_literals;
                buffer[i] = (float)buffer_i8[i * 2] + (float)buffer_i8[i * 2 + 1] * 1if;
            }
        }

        // Agc
        agc.work(buffer, BUFFER_SIZE, agc_buff);

        // Carrier-tracking PLL
        pll.work(agc_buff, BUFFER_SIZE, pll_buff);

        // Average
        int numMovingAvg = movingAverage.work(pll_buff, BUFFER_SIZE, moving_buff);

        // This is pure junk and slow as heck, but it got the thing working for now
        for (int i = 0; i < numMovingAvg; i++)
            clockRecoIn.push_back(std::complex<float>(moving_buff[i], 0));
        clockRecoOut = clockReco.work(clockRecoIn, clockRecoIn.size());
        int recovered_size = clockRecoOut.size();
        for (int i = 0; i < recovered_size; i++)
            recovered_buff[i] = clockRecoOut[i].real();

        // Deframe
        if (recovered_size > 0)
        {
            volk_32f_binary_slicer_8i_generic((int8_t *)bitsBuffer, recovered_buff, recovered_size);
            frames = deframer.work(bitsBuffer, recovered_size);
        }

        // Count frames
        frame_count += frames.size();

        // Write to file
        if (frames.size() > 0)
            for (uint16_t &frame : frames)
                data_out.write((char *)&frame, sizeof(uint16_t));

        // Console stuff
        std::cout << '\r' << "Frames : " << frame_count / 11090 << ", Data out : " << round(frame_count * 2 / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;

        // Clear vectors
        frames.clear();
        clockRecoOut.clear();
        clockRecoIn.clear();
    }

    data_in.close();
    data_out.close();
}