#include "demuxer.h"
#include <vector>
#include "mpdu.h"
#include <cstring>

#include <iostream>
#include <bitset>

#define HEADER_LENGTH 6
#define MPDU_DATA_SIZE 882

namespace libccsds
{
    Demuxer::Demuxer()
    {
        // Init variables
        currentCCSDSPacket.header.packet_length = 0;
        currentPacketPayloadLength = 0;
        remainingPacketLength = 0;
        totalPacketLength = 0;
        inHeader = false;
        inHeaderBuffer = 0;
        abortPacket();
    }

    // Fill header and packet lengths
    void Demuxer::readPacket(uint8_t *h)
    {
        workingOnPacket = true;
        currentCCSDSPacket.header = parseCCSDSHeader(h);
        currentPacketPayloadLength = currentCCSDSPacket.header.packet_length + 1;
        totalPacketLength = currentPacketPayloadLength + HEADER_LENGTH;
        remainingPacketLength = currentPacketPayloadLength;
    }

    // Clear everything and push into packet buffer
    void Demuxer::pushPacket()
    {
        ccsdsBuffer.push_back(currentCCSDSPacket);
        currentCCSDSPacket.payload.clear();
        currentCCSDSPacket.header.packet_length = 0;
        currentPacketPayloadLength = 0;
        remainingPacketLength = 0;
        workingOnPacket = false;
    }

    // Push data into the packet
    void Demuxer::pushPayload(uint8_t *data, int length)
    {
        for (int i = 0; i < length; i++)
            currentCCSDSPacket.payload.push_back(data[i]);

        remainingPacketLength -= length;
    }

    // ABORT!
    void Demuxer::abortPacket()
    {
        workingOnPacket = false;
        currentCCSDSPacket.payload.clear();
        currentCCSDSPacket.header.packet_length = 0;
        currentPacketPayloadLength = 0;
        remainingPacketLength = 0;
    };

    std::vector<CCSDSPacket> Demuxer::work(CADU &cadu)
    {
        ccsdsBuffer.clear(); // Clear buffer from previous run

        MPDU mpdu = parseMPDU(cadu); // Parse M-PDU Header

        int offset = 0;

        // We're parsing a header!
        if (inHeader)
        {
            inHeader = false;
            std::memcpy(&headerBuffer[inHeaderBuffer], &mpdu.data[0], 6 - inHeaderBuffer);
            offset = (7 - inHeaderBuffer) - 1;
            inHeaderBuffer += 6 - inHeaderBuffer;

            // Parse it
            readPacket(headerBuffer);
        }

        if (remainingPacketLength > 0 && workingOnPacket)
        {
            if (mpdu.first_header_pointer < 2047)
            {
                int toWrite = remainingPacketLength > mpdu.first_header_pointer + 1 ? mpdu.first_header_pointer + 1 : remainingPacketLength;
                pushPayload(&mpdu.data[offset], toWrite);
                remainingPacketLength = 0;
            }
            else
            {
                int toWrite = remainingPacketLength > MPDU_DATA_SIZE ? MPDU_DATA_SIZE : remainingPacketLength;
                pushPayload(&mpdu.data[offset], toWrite);
            }
        }

        if (remainingPacketLength == 0 && workingOnPacket)
        {
            pushPacket();
        }

        if (mpdu.first_header_pointer < 2047)
        {
            readPacket(&mpdu.data[mpdu.first_header_pointer]);

            bool hasSecondHeader = MPDU_DATA_SIZE >= mpdu.first_header_pointer + totalPacketLength;

            // A second header can fit, so search for it!
            if (hasSecondHeader)
            {
                if (mpdu.first_header_pointer + totalPacketLength <= MPDU_DATA_SIZE)
                {
                    pushPayload(&mpdu.data[mpdu.first_header_pointer + 6], currentPacketPayloadLength);
                    pushPacket();
                }
                else
                {
                    abortPacket();
                }

                // Compute next possible header
                int nextHeaderPointer = mpdu.first_header_pointer + totalPacketLength;

                while (nextHeaderPointer + 1 <= MPDU_DATA_SIZE)
                {
                    // The header fits!
                    if (nextHeaderPointer + HEADER_LENGTH + 1 <= MPDU_DATA_SIZE)
                    {
                        readPacket(&mpdu.data[nextHeaderPointer]);

                        int toWrite = remainingPacketLength > (MPDU_DATA_SIZE - (nextHeaderPointer + 6)) ? (MPDU_DATA_SIZE - (nextHeaderPointer + 6)) : remainingPacketLength;
                        pushPayload(&mpdu.data[nextHeaderPointer + 6], toWrite);
                    }
                    // Only part of the header fits. At least 1 byte has to be there or it'll be in the next frame
                    else if (nextHeaderPointer + 1 <= MPDU_DATA_SIZE)
                    {
                        inHeader = true;
                        inHeaderBuffer = 0;
                        std::memcpy(headerBuffer, &mpdu.data[nextHeaderPointer], (MPDU_DATA_SIZE - 1) - nextHeaderPointer);
                        inHeaderBuffer += MPDU_DATA_SIZE - nextHeaderPointer;
                        break;
                    }

                    // Update to next pointer
                    nextHeaderPointer = nextHeaderPointer + totalPacketLength;
                }
            }
            else
            {
                int toWrite = remainingPacketLength > (MPDU_DATA_SIZE - (mpdu.first_header_pointer + 6)) ? (MPDU_DATA_SIZE - (mpdu.first_header_pointer + 6)) : remainingPacketLength;
                pushPayload(&mpdu.data[mpdu.first_header_pointer + 6], toWrite);
            }
        }

        return ccsdsBuffer;
    } // namespace libccsds
} // namespace libccsds