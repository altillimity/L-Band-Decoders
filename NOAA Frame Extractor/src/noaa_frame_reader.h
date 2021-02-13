#pragma once

#include <fstream>
#include <vector>
#include <array>

#define BUFFER_SIZE (11090 * 2)

class NOAAFrameReader
{
private:
    std::array<uint8_t, 104> frameBuffer;
    template <typename T>
    inline bool getBit(T &data, int bit);

public:
    NOAAFrameReader();
    int frames;
    std::vector<std::array<uint8_t, 104>> readFrames(std::ifstream inputStream, int frame);
};