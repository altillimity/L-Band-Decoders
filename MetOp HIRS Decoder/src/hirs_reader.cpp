#include "hirs_reader.h"

HIRSReader::HIRSReader()
{
    for (int i = 0; i < 20; i++)
        channels[i] = new unsigned short[10000 * 56];
    lines = 0;
}

uint16_t reverse16Bits(uint16_t v)
{
    uint16_t r = 0;
    for (int i = 0; i < 16; ++i, v >>= 1)
        r = (r << 1) | (v & 0x01);
    return r;
}

/** Shift an array right.
 * @param ar The array to shift.
 * @param size The number of array elements.
 * @param shift The number of bits to shift.
 */
void shift_right(unsigned char *ar, int size, int shift)
{
    int carry = 0; // Clear the initial carry bit.
    while (shift--)
    { // For each bit to shift ...
        for (int i = size - 1; i >= 0; --i)
        {                                      // For each element of the array from high to low ...
            int next = (ar[i] & 1) ? 0x80 : 0; // ... if the low bit is set, set the carry bit.
            ar[i] = carry | (ar[i] >> 1);      // Shift the element one bit left and addthe old carry.
            carry = next;                      // Remember the old carry for next time.
        }
    }
}

template <typename T>
constexpr void shift_array_left(T *arr, const size_t size, const size_t bits, const bool zero = false)
{
    const size_t chunks = bits / (8 * sizeof(T));

    if (chunks >= size)
    {
        if (zero)
        {
            memset(arr, 0, size);
        }
        return;
    }

    if (chunks)
    {
        memmove(arr, arr + chunks, size - chunks);
        if (zero)
        {
            memset(arr + size - chunks, 0, chunks);
        }
    }

    const size_t left = bits % (8 * sizeof(T));

    // If we have non directly addressable bits left we need to move the whole thing one by one.
    if (left)
    {
        const size_t right = (8 * sizeof(T)) - left;
        const size_t l = size - chunks - 1;
        for (size_t i = 0; i < l; i++)
        {
            arr[i] = ((arr[i] << left) & ~left) | (arr[i + 1] >> right);
        }
        arr[l] = (arr[l] << left) & ~left;
    }
}

void HIRSReader::work(libccsds::CCSDSPacket &packet)
{
    if (packet.payload.size() < 2320)
        return;

    int pos = 14;

    shift_array_left<uint8_t>(packet.payload.data(), 2320, 27);

    for (int i = 0; i < 56 * 60; i += 20)
    {
        lineBuffer[i + 0] = (packet.payload[pos - 2] & 0b00011111) << 8 | packet.payload[pos - 1];                                          // Channel 1
        lineBuffer[i + 1] = packet.payload[pos + 0] << 5 | packet.payload[pos + 1] >> 3;                                                    // Channel 2
        lineBuffer[i + 2] = (packet.payload[pos + 1] & 0x07) << 10 | packet.payload[pos + 2] << 2 | packet.payload[pos + 3] >> 6;           // Channel 3
        lineBuffer[i + 3] = (packet.payload[pos + 3] & 0b00111111) << 7 | packet.payload[pos + 4] >> 1;                                     // Channel 4
        lineBuffer[i + 4] = (packet.payload[pos + 4] & 0b00000001) << 12 | packet.payload[pos + 5] << 4 | packet.payload[pos + 6] >> 4;     // Channel 5
        lineBuffer[i + 5] = (packet.payload[pos + 6] & 0b00001111) << 9 | packet.payload[pos + 7] << 1 | packet.payload[pos + 8] >> 7;      // Channel 6
        lineBuffer[i + 6] = (packet.payload[pos + 8] & 0b01111111) << 6 | packet.payload[pos + 9] >> 2;                                     // Channel 7
        lineBuffer[i + 7] = (packet.payload[pos + 9] & 0b00000011) << 11 | packet.payload[pos + 10] << 3 | packet.payload[pos + 11] >> 5;   // Channel 8
        lineBuffer[i + 8] = (packet.payload[pos + 11] & 0b00011111) << 8 | packet.payload[pos + 12];                                        // Channel 9
        lineBuffer[i + 9] = packet.payload[pos + 13] << 5 | packet.payload[pos + 14] >> 3;                                                  // Channel 10
        lineBuffer[i + 10] = (packet.payload[pos + 14] & 0x07) << 10 | packet.payload[pos + 15] << 2 | packet.payload[pos + 16] >> 6;       // Channel 11
        lineBuffer[i + 11] = (packet.payload[pos + 16] & 0b00111111) << 7 | packet.payload[pos + 17] >> 1;                                  // Channel 12
        lineBuffer[i + 12] = (packet.payload[pos + 17] & 0b00000001) << 12 | packet.payload[pos + 18] << 4 | packet.payload[pos + 19] >> 4; // Channel 13
        lineBuffer[i + 13] = (packet.payload[pos + 19] & 0b00001111) << 9 | packet.payload[pos + 20] << 1 | packet.payload[pos + 21] >> 7;  // Channel 14
        lineBuffer[i + 14] = (packet.payload[pos + 21] & 0b01111111) << 6 | packet.payload[pos + 22] >> 2;                                  // Channel 15
        lineBuffer[i + 15] = (packet.payload[pos + 22] & 0b00000011) << 11 | packet.payload[pos + 23] << 3 | packet.payload[pos + 24] >> 5; // Channel 16
        lineBuffer[i + 16] = (packet.payload[pos + 24] & 0b00011111) << 8 | packet.payload[pos + 25];                                       // Channel 17
        lineBuffer[i + 17] = packet.payload[pos + 26] << 5 | packet.payload[pos + 27] >> 3;                                                 // Channel 18
        lineBuffer[i + 18] = (packet.payload[pos + 27] & 0x07) << 10 | packet.payload[pos + 28] << 2 | packet.payload[pos + 29] >> 6;       // Channel 19
        lineBuffer[i + 19] = (packet.payload[pos + 29] & 0b00111111) << 7 | packet.payload[pos + 30] >> 1;                                  // Channel 20

        pos += 36;
    }

    // Plot into an image
    for (int channel = 0; channel < 20; channel++)
    {
        for (int i = 0; i < 56; i++)
        {
            channels[channel][lines * 56 + i] = (lineBuffer[i * 20 + channel]);
        }
    }

    // Frame counter
    lines++;
}

cimg_library::CImg<unsigned short> HIRSReader::getChannel(int channel)
{
    cimg_library::CImg<unsigned short> img = cimg_library::CImg<unsigned short>(channels[channel], 56, lines);
    img.normalize(0, 65535);
    //img.equalize(1000);
    return img;
}