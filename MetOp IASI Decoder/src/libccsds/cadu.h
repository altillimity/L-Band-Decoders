#pragma once

#include <cstdint>
#include <array>

#define CADU_SIZE 1024

namespace libccsds
{
    // Convenience only
    typedef std::array<uint8_t, CADU_SIZE> CADU;
} // namespace libccsds
