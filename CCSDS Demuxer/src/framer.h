#pragma once

/*
    A CCSDS Framer that takes CADUs as input
*/

#include <vector>
#include <array>
#include "ccsds.h"

class CCSDSFramer
{
private:
    // Buffer
    std::vector<uint8_t> ccsdsBuffer;
    // Sorting options
    int select_vcid, select_apid, select_frame_size;
    // CCSDS Frame counter used initially
    int ccsdsNum;

public:
    CCSDSFramer(int vcid, int apid, int select_frame_size = 0);
    std::vector<std::vector<uint8_t>> work(std::vector<std::array<uint8_t, CADU_SIZE>> cadus);
};