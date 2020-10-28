#include <iostream>
#include <fstream>
#include <complex>
#include <math.h>
#include <vector>
#include <cassert>
#include "agc.h"
#include "rrc_filter.h"
#include "costas.h"
#include "clock_recovery.h"
#include "tclap/CmdLine.h"

#define BUFFER_SIZE (8192 * 10)

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
    TCLAP::CmdLine cmd("QPSK Demodulator by Aang23", ' ', "1.0");

    // File arguments
    TCLAP::ValueArg<std::string> valueInput("i", "input", "Baseband input", true, "", "baseband.raw");
    TCLAP::ValueArg<std::string> valueOutput("o", "output", "Output synced symbols", true, "", "symbols.bin");

    // VCID / APID to extract
    TCLAP::ValueArg<int> valueSamplerate("s", "samplerate", "Baseband samplerate", true, 6000000, "6000000");
    TCLAP::ValueArg<int> valueSymbolrate("b", "symbolrate", "QPSK Symbolrate", true, 2800000, "2800000");

    // Baseband format
    TCLAP::SwitchArg valueF32Baseband("f", "float32", "Input baseband as float32");
    TCLAP::SwitchArg valueInt16Baseband("6", "int16", "Input baseband as int16");
    TCLAP::SwitchArg valueInt8Baseband("8", "int8", "Input baseband as int8");
    TCLAP::SwitchArg valueWav8Baseband("e", "wav8", "Input baseband as wav8");

    // Register all of the above options
    cmd.add(valueInput);
    cmd.add(valueOutput);
    cmd.add(valueSamplerate);
    cmd.add(valueSymbolrate);
    cmd.add(valueF32Baseband);
    cmd.add(valueInt16Baseband);
    cmd.add(valueInt8Baseband);
    cmd.add(valueWav8Baseband);

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
    if (!(valueF32Baseband.getValue() || valueInt16Baseband.getValue() || valueInt8Baseband.getValue() || valueWav8Baseband.getValue()))
    {
        std::cout << "Please specify baseband format! -f, -6, -8" << std::endl;
        return 1;
    }
    else if ((valueF32Baseband.getValue() + valueInt16Baseband.getValue() + valueInt8Baseband.getValue() + valueWav8Baseband.getValue()) > 1)
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

    // "Blocks" :)
    Agc agc(AGC_WINSIZE, 1, AGC_TARGET);
    RRCFilter rrc(180, 1, (float)valueSamplerate.getValue() / (float)valueSymbolrate.getValue(), 0.5f);
    Costas pll(0.005f, 4);
    ClockRecovery clockreco(valueSamplerate.getValue(), valueSymbolrate.getValue());

    // All buffers we use along the way
    std::complex<float> *buffer = new std::complex<float>[BUFFER_SIZE],
                        *agc_buffer = new std::complex<float>[BUFFER_SIZE],
                        *filter_buffer = new std::complex<float>[BUFFER_SIZE],
                        *recovery_buffer = new std::complex<float>[BUFFER_SIZE];

    // Int16 buffer
    int16_t *buffer_i16 = new int16_t[BUFFER_SIZE * 2];

    // Int8 buffer
    int8_t *buffer_i8 = new int8_t[BUFFER_SIZE * 2];

    // Uint8 buffer
    uint8_t *buffer_u8 = new uint8_t[BUFFER_SIZE * 2];

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
        else if (valueWav8Baseband.getValue())
        {
            data_in.read((char *)buffer_u8, BUFFER_SIZE * sizeof(uint8_t) * 2);
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                float imag = (buffer_u8[i * 2] - 127) * 0.004f;
                float real = (buffer_u8[i * 2 + 1] - 127) * 0.004f;
                using namespace std::complex_literals;
                buffer[i] = real + imag * 1if;
            }
        }

        // AGC
        agc.work(buffer, BUFFER_SIZE, agc_buffer);

        // Root-raised-cosine filtering
        rrc.work(agc_buffer, BUFFER_SIZE, filter_buffer);

        // Costas loop, frequency offset recovery
        pll.work(filter_buffer, BUFFER_SIZE);

        // Clock recovery
        int recovered_size = clockreco.work(filter_buffer, BUFFER_SIZE, recovery_buffer);

        // Write to output
        data_out.write((char *)recovery_buffer, recovered_size * sizeof(std::complex<float>));
        dataout += recovered_size * sizeof(std::complex<float>);

        // Console stuff
        std::cout << '\r' << "Data out : " << round(dataout / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }
    data_in.close();
    data_out.close();
}