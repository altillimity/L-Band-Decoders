#pragma once

#include <stdint.h>
#include "ccsds.h"
#include <vector>
#include <array>

/*
    A CADU Reed-Solomon corrector
*/

class CADUReedSolomon
{
private:
    // Internal variables
    uint8_t rsBuffer[255];
    bool corrected = false;
    int i, j, result;

public:
    std::vector<std::array<uint8_t, CADU_SIZE>> work(std::vector<std::array<uint8_t, CADU_SIZE>> &frames, int& errored_frames);
};