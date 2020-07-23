#include "deframer.h"

// 3 States
#define STATE_NOSYNC 0
#define STATE_SYNCING 1
#define STATE_SYNCED 2

// Returns the asked bit!
template <typename T>
inline bool getBit(T &data, int &bit)
{
    return (data >> bit) & 1;
}

// Compare 2 32-bits values bit per bit
int checkSyncMarker(uint32_t &marker, uint32_t totest)
{
    int errors = 0;
    for (int i = 31; i >= 0; i--)
    {
        bool markerBit, testBit;
        markerBit = getBit<uint32_t>(marker, i);
        testBit = getBit<uint32_t>(totest, i);
        if (markerBit != testBit)
            errors++;
    }
    return errors;
}

CADUDeframer::CADUDeframer()
{
    // From gr-poes-weather (including comments!)
    unsigned char feedbk, randm = 0xff;
    // Original Polynomial is :  1 + x3 + x5 + x7 +x8
    d_rantab[0] = 0;
    d_rantab[1] = 0;
    d_rantab[2] = 0;
    d_rantab[3] = 0;
    for (int i = 4; i < CADU_SIZE; i++)
    { //4ASM bytes + 1020bytes = 32 + 8160 bits in CADU packet

        d_rantab[i] = 0;
        for (int j = 0; j <= 7; j++)
        {
            d_rantab[i] = d_rantab[i] << 1;
            if (randm & 0x80) //80h = 1000 0000b
                d_rantab[i]++;

            //Bit-Wise AND between: Fixed shift register(95h) and the state of the
            // feedback register: randm
            feedbk = randm & 0x95; //95h = 1001 0101--> bits 1,3,5,8
            //feedback contains the contents of the registers masked by the polynomial
            //  1 + x3 + x5 + xt +x8 = 95 h
            randm = randm << 1;

            if ((((feedbk & 0x80) ^ (0x80 & feedbk << 3)) ^ (0x80 & (feedbk << 5))) ^ (0x80 & (feedbk << 7)))
                randm++;
        }
    }

    // Default values
    writeFrame = false;
    numFrames = 0;
    wroteBits = 8;
    wroteBytes = 0;
    skip = 0;
    good = 0;
    errors = 0;
    state = STATE_NOSYNC;
    bit_inversion = false;
}

int CADUDeframer::getFrameCount()
{
    return numFrames;
}

int CADUDeframer::getState()
{
    return state;
}

std::vector<std::array<uint8_t, CADU_SIZE>> CADUDeframer::work(uint8_t *input, size_t size)
{
    // Output buffer
    std::vector<std::array<uint8_t, CADU_SIZE>> frames;

    // Loop in all bytes
    for (int byteInBuf = 0; byteInBuf < size; byteInBuf++)
    {
        // Loop in all bits!
        for (int i = 7; i >= 0; i--)
        {
            // Get a bit, perform bit inversion if necessary
            uint8_t bit = bit_inversion ? !getBit<uint8_t>(input[byteInBuf], i) : getBit<uint8_t>(input[byteInBuf], i);
            // Push it into out shifter
            shifter = (shifter << 1) | bit;

            // Are we writing a frame?
            if (writeFrame)
            {
                // First loop : add clean ASM Marker
                if (wroteBytes == 0)
                {
                    frameBuffer[0] = CADU_ASM_1;
                    frameBuffer[1] = CADU_ASM_2;
                    frameBuffer[2] = CADU_ASM_3;
                    frameBuffer[3] = CADU_ASM_4;
                    wroteBytes += 4;
                }

                // Push bit into out own 1-byte shifter
                outBuffer = (outBuffer << 1) | bit;

                // If we filled the buffer, output it
                if (--wroteBits == 0)
                {
                    // Derandomization
                    frameBuffer[wroteBytes] = outBuffer ^ d_rantab[wroteBytes];
                    wroteBytes++;
                    wroteBits = 8;
                }

                // Did we write the entire frame?
                if (wroteBytes == CADU_SIZE)
                {
                    // Exit of this loop, reset values and push the frame
                    writeFrame = false;
                    wroteBits = 8;
                    wroteBytes = 0;
                    skip = CADU_ASM_SIZE * 8; // Push back next ASM in shifter
                    frames.push_back(frameBuffer);
                }

                continue;
            }

            // Skip a few run if necessary
            if (skip > 1)
            {
                skip--;
                continue;
            }
            else if (skip == 1) // Last run should NOT reset the loop
                skip--;

            // Initial state : STATE_NOSYNC
            // We search for a perfectly matching sync word, 1 found and we start STATE_SYNCING
            if (state == STATE_NOSYNC)
            {
                if (shifter == CADU_ASM) // ASM
                {
                    numFrames++;
                    state = STATE_SYNCING;
                    writeFrame = true;
                    errors = 0;
                }
                else if (shifter == CADU_ASM_INV) // NASM to handle bit inversion
                {
                    numFrames++;
                    state = STATE_SYNCING;
                    writeFrame = true;
                    errors = 0;
                    bit_inversion = true;
                }
            }
            // Secondary state : STATE_SYNCING
            // We allow a few more mismatches and scan frame-per-frame
            // 10 good frames and we get into STATE_SYNCED, 4 errors and we go back to STATE_NOSYNC
            else if (state == STATE_SYNCING)
            {
                if (checkSyncMarker(shifter, CADU_ASM) <= 5)
                {
                    numFrames++;
                    writeFrame = true;
                    errors = 0;
                    good++;
                }
                else
                {
                    skip = (CADU_SIZE)*8;
                    errors++;
                }

                if (errors > 3)
                {
                    state = STATE_NOSYNC;
                    skip = 0;
                }

                if (good == 10)
                {
                    state = STATE_SYNCED;
                    good = 0;
                    errors = 0;
                }
            }
            // Third state : STATE_SYNCED
            // We assume perfect sync and allow very high ASM mismatches
            // A single error and we go back to STATE_SYNCING
            else if (state == STATE_SYNCED)
            {
                if (checkSyncMarker(shifter, CADU_ASM) <= 17)
                {
                    numFrames++;
                    writeFrame = true;
                    errors = 0;
                }
                else
                {
                    skip = (CADU_SIZE)*8;
                    errors++;
                }

                if (errors > 0)
                {
                    state = STATE_SYNCING;
                }
            }
        }
    }

    // Output what we found if anything
    return frames;
}