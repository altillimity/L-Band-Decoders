#pragma once

#include <cstdint>
#include <vector>
#include "cadu.h"

namespace libccsds
{
    // Struct representing a M-PDU
    struct MPDU
    {
        uint16_t first_header_pointer;
        uint8_t *data;
    };

    // Parse MPDU from CADU
    MPDU parseMPDU(CADU &cadu);

} // namespace libccsds
