#include "framer.h"

#include <math.h>

CCSDSFramer::CCSDSFramer(int vcid, int apid, int frame_size)
    : select_vcid(vcid),
      select_apid(apid),
      select_frame_size(frame_size)
{
}

std::vector<std::vector<uint8_t>> CCSDSFramer::work(std::vector<std::array<uint8_t, CADU_SIZE>> cadus)
{
    // Return buffer
    std::vector<std::vector<uint8_t>> returnFrames;

    // Loop between all M-PDUs
    for (std::array<uint8_t, CADU_SIZE> &frame : cadus)
    {
        // Extract VCID
        int vcid = frame[5] % ((int)pow(2, 6));

        // Check this VCID is what we want
        if (vcid == select_vcid)
        {
            // Extract CCSDS next frame pointer
            uint16_t firstCCSDSPointer = ((frame[12] % ((int)pow(2, 3))) << 8) | frame[13];

            // All bits set to 11 if there is no header, so test that
            if (firstCCSDSPointer < 2047)
            {
                // Make sure to run ONLY after we found a first header...
                if (ccsdsNum < 3)
                    ccsdsNum++;

                // Only read frame end after we wrote at least one...
                if (ccsdsNum > 2)
                    for (int i = 0; i < firstCCSDSPointer; i++)
                        ccsdsBuffer.push_back(frame[14 + i]);

                // Read APID
                int apid = ((ccsdsBuffer[0] % ((int)pow(2, 3))) << 8) | ccsdsBuffer[1];

                // Check APID and request frame length (if any) match. If so, push!
                if (apid = select_apid && (select_frame_size == 0 || ccsdsBuffer.size() == select_frame_size))
                    returnFrames.push_back(ccsdsBuffer);

                // Clear CCSDS buffer
                ccsdsBuffer.clear();

                for (int i = firstCCSDSPointer; i < 882; i++)
                    ccsdsBuffer.push_back(frame[14 + i]);
            }
            // If we ran at least once and there is no header well... We just have to copy all the data!
            else if (ccsdsNum > 1)
                for (int i = 0; i < 882; i++)
                    ccsdsBuffer.push_back(frame[14 + i]);
        }
    }

    return returnFrames;
}