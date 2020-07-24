#include "simpledeframer.h"

#include <math.h>

// Returns the asked bit!
template <typename T>
inline bool getBit(T &data, int &bit)
{
    return (data >> bit) & 1;
}

template <typename SYNC_T, int SYNC_SIZE, int FRAME_SIZE, SYNC_T ASM_SYNC>
SimpleDeframer<SYNC_T, SYNC_SIZE, FRAME_SIZE, ASM_SYNC>::SimpleDeframer()
{
    // Default values
    writeFrame = false;
    wroteBits = 0;
    outputBits = 0;
}

// Write a single bit into the frame
template <typename SYNC_T, int SYNC_SIZE, int FRAME_SIZE, SYNC_T ASM_SYNC>
void SimpleDeframer<SYNC_T, SYNC_SIZE, FRAME_SIZE, ASM_SYNC>::pushBit(uint8_t bit)
{
    byteBuffer = (byteBuffer << 1) | bit;
    wroteBits++;
    if (wroteBits == 8)
    {
        frameBuffer.push_back(byteBuffer);
        wroteBits = 0;
    }
}

template <typename SYNC_T, int SYNC_SIZE, int FRAME_SIZE, SYNC_T ASM_SYNC>
std::vector<std::vector<uint8_t>> SimpleDeframer<SYNC_T, SYNC_SIZE, FRAME_SIZE, ASM_SYNC>::work(std::vector<std::vector<uint8_t>> &frames)
{
    // Output buffer
    std::vector<std::vector<uint8_t>> framesOut;

    // Loop in all frames
    for (std::vector<uint8_t> &frame : frames)
    {
        // Loop in all bytes
        for (uint8_t &byte : frame)
        {
            // Loop in all bits!
            for (int i = 7; i >= 0; i--)
            {
                // Get a bit, push it
                uint8_t bit = getBit<uint8_t>(byte, i);
                shifter = ((shifter << 1) % (long)pow(2, SYNC_SIZE)) | bit;

                // Writing a frame!
                if (writeFrame)
                {
                    // First run : push header
                    if (outputBits == 0)
                    {
                        SYNC_T syncAsm = ASM_SYNC;
                        for (int y = SYNC_SIZE; y >= 0; y--)
                        {
                            pushBit(getBit<uint64_t>(syncAsm, y));
                            outputBits++;
                        }
                    }

                    // Push current bit
                    pushBit(bit);
                    outputBits++;

                    // Once we wrote a frame, exit!
                    if (outputBits == FRAME_SIZE)
                    {
                        writeFrame = false;
                        wroteBits = 0;
                        outputBits = 0;
                        framesOut.push_back(frameBuffer);
                        frameBuffer.clear();
                    }

                    continue;
                }

                // Otherwise search for markers
                if (shifter == ASM_SYNC)
                {
                    writeFrame = true;
                }
            }
        }
    }

    // Output what we found if anything
    return framesOut;
}

// Build this template for FY
template class SimpleDeframer<uint64_t, 60, 208400, 0b101000010001011011111101011100011001110110000011110010010101>;