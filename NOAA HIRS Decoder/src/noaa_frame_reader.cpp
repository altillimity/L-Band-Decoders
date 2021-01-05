#include "noaa_frame_reader.h"

NOAAFrameReader::NOAAFrameReader()
{
    frames = 0;
}

template <typename T>
inline bool NOAAFrameReader::getBit(T &data, int bit)
{
    return (data >> bit) & 1;
}

std::vector<std::array<uint8_t, 104>> NOAAFrameReader::readFrames(std::ifstream inputStream, int frame)
{
    uint16_t buffer[BUFFER_SIZE / 2];
    std::vector<std::array<uint8_t, 104>> out;
    while (!inputStream.eof())
    {
        // Read buffer
        inputStream.read((char *)buffer, BUFFER_SIZE);

        //get the HRPT minor frame number
        bool bit1 = getBit(buffer[6], 8);
        bool bit2 = getBit(buffer[6], 7);
        int frmNum = bit1 << 1 | bit2;
        //init the MHS buffer
        if (frmNum == frame)
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 104; j++){
                    frameBuffer[j] = buffer[104 * (i+1) + j - 1] >> 2;
                }
                out.push_back(frameBuffer);
            }
        }

    }
    inputStream.close();
    return out;
}