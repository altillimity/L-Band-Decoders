#include "reedsolomon.h"

extern "C"
{
#include <fec.h>
}

std::vector<std::array<uint8_t, CADU_SIZE>> CADUReedSolomon::work(std::vector<std::array<uint8_t, CADU_SIZE>> &frames, int &errored_frames)
{
    // Output buffer
    std::vector<std::array<uint8_t, CADU_SIZE>> correctedFrames;

    // Loop through everything provided
    for (std::array<uint8_t, CADU_SIZE> &frame : frames)
    {
        corrected = true;

        // Deinterleave and correct
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 255; j++)
                rsBuffer[j] = (&frame[CADU_ASM_SIZE])[i + j * 4];

            // Perform CCSDS RS
            result = decode_rs_ccsds(rsBuffer, NULL, 0, 0);
            if (result == -1)
                corrected = false; // Any error and we discard the frame

            for (j = 0; j < 255; j++)
                (&frame[CADU_ASM_SIZE])[i + j * 4] = rsBuffer[j];
        }

        // Push back good frames or increment errors
        if (corrected)
            correctedFrames.push_back(frame);
        else
            errored_frames++;
    }

    // Return it all
    return correctedFrames;
}