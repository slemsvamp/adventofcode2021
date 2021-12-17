#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "common.h"

const u32 MAX_PACKET_COUNT = 100;
const u32 MAX_SUBPACKET_COUNT = 100;

struct packet
{
    u8 Version;
    u8 TypeId;
    u64 Value;

    struct packet *Subpackets;
    u32 SubpacketCount;
};

struct packets_info
{
    packet *Packets;
    u32 PacketCount;
};

struct parse_result
{
    packets_info PacketsInfo;
};

internal packets_info RecursivePacketSniffer(char **buffer, char *endAddress, u32 packetReadCount);
internal u64 Execute(packet p);

u32 _versionSum = 0;
u32 _depth = 0;

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    u32 bufferSize = file.Size * 4;

    char *buffer = (char *)calloc(bufferSize, 1);
    u32 bufferCount = 0;

    char *playhead = file.Data;
    while (playhead - file.Data < file.Size)
    {
        char bits[4] = {0, 0, 0, 0};
        switch (*playhead)
        {
            case '0': break;
            case '1': bits[3] = 1; break;
            case '2': bits[2] = 1; break;
            case '3': bits[2] = 1; bits[3] = 1; break;
            case '4': bits[1] = 1; break;
            case '5': bits[1] = 1; bits[3] = 1; break;
            case '6': bits[1] = 1; bits[2] = 1; break;
            case '7': bits[1] = 1; bits[2] = 1; bits[3] = 1; break;
            case '8': bits[0] = 1; break;
            case '9': bits[0] = 1; bits[3] = 1; break;
            case 'A': bits[0] = 1; bits[2] = 1; break;
            case 'B': bits[0] = 1; bits[2] = 1; bits[3] = 1; break;
            case 'C': bits[0] = 1; bits[1] = 1; break;
            case 'D': bits[0] = 1; bits[1] = 1; bits[3] = 1; break;
            case 'E': bits[0] = 1; bits[1] = 1; bits[2] = 1; break;
            case 'F': bits[0] = 1; bits[1] = 1; bits[2] = 1; bits[3] = 1; break;
            InvalidDefaultCase;
        };

        *(buffer + bufferCount++) = bits[0];
        *(buffer + bufferCount++) = bits[1];
        *(buffer + bufferCount++) = bits[2];
        *(buffer + bufferCount++) = bits[3];

        playhead++;
    }

    playhead = buffer;
    
    result.PacketsInfo = RecursivePacketSniffer(&playhead, buffer + bufferSize, 0);

    free(buffer);
    return result;
}

internal u64
GetLiteral(char **playheadPointer)
{
    u64 result = 0;
    char *playhead = *playheadPointer;
    u8 *literalBuffer = (u8 *)calloc(64, sizeof(u8));
    u32 literalBufferCount = 0;
    b8 processLiteral = true;

    while (processLiteral)
    {
        if (!*playhead)
            processLiteral = false;
        *(literalBuffer + literalBufferCount++) = *(playhead + 1);
        *(literalBuffer + literalBufferCount++) = *(playhead + 2);
        *(literalBuffer + literalBufferCount++) = *(playhead + 3);
        *(literalBuffer + literalBufferCount++) = *(playhead + 4);
        playhead += 5;
    }
    
    for (u32 processLiteralIndex = 0; processLiteralIndex < literalBufferCount; processLiteralIndex++)
        result += (u64)*(literalBuffer + processLiteralIndex) << (literalBufferCount - processLiteralIndex - 1);

    free(literalBuffer);

    *playheadPointer = playhead;

    return result;
}

internal u64
GetNumber(char **playheadPointer, u32 bitCount)
{
    u64 result = 0;
    char *playhead = *playheadPointer;
    for (u32 bitIndex = 0; bitIndex < bitCount; bitIndex++)
        result += *(playhead + bitIndex) << (bitCount - bitIndex - 1);
    *playheadPointer = (playhead + bitCount);

    return result;
}

internal packets_info
RecursivePacketSniffer(char **buffer, char *endAddress, u32 packetReadCount)
{
    packets_info result;

    packet *packetBuffer = (packet *)calloc(MAX_PACKET_COUNT, sizeof(packet));
    u32 packetCount = 0;

    char *playhead = *buffer;

    while ((endAddress && playhead + 7 < endAddress) || (!endAddress && packetReadCount > 0))
    {
        if (packetReadCount > 0)
            packetReadCount--;

        Assert(packetCount < MAX_PACKET_COUNT);

        packet p = {};

        p.Version = (*playhead << 2) + (*(playhead + 1) << 1) + *(playhead + 2);
        playhead += 3;
        
        p.TypeId = (*playhead << 2) + (*(playhead + 1) << 1) + *(playhead + 2);
        playhead += 3;

        if (p.TypeId == 4)
            p.Value = GetLiteral(&playhead);
        else
        {
            u8 lengthTypeId = *playhead;
            playhead++;
            
            packets_info subpacketsInfo;
            if (!lengthTypeId)
                subpacketsInfo = RecursivePacketSniffer(&playhead, playhead + GetNumber(&playhead, 15), 0);
            else
                subpacketsInfo = RecursivePacketSniffer(&playhead, 0, GetNumber(&playhead, 11));

            p.Subpackets = subpacketsInfo.Packets;
            p.SubpacketCount = subpacketsInfo.PacketCount;
        }

        _versionSum += p.Version;

        *(packetBuffer + packetCount++) = p;
    }

    result.Packets = (packet *)calloc(packetCount, sizeof(packet));
    memcpy(result.Packets, packetBuffer, packetCount * sizeof(packet));
    result.PacketCount = packetCount;

    *buffer = playhead;

    free(packetBuffer);
    return result;
}

internal u64
PacketsSum(packet *packets, u32 packetCount)
{
    u64 result = 0;
    for (u32 packetIndex = 0; packetIndex < packetCount; packetIndex++)
    {
        packet p = *(packets + packetIndex);
        u64 value = Execute(p);
        result += value;
    }
    return result;
}

internal u64
PacketsProduct(packet *packets, u32 packetCount)
{
    u64 result = 1;
    for (u32 packetIndex = 0; packetIndex < packetCount; packetIndex++)
    {
        packet p = *(packets + packetIndex);
        u64 value = Execute(p);
        result *= value;
    }
    return result;
}

internal u64
PacketsMin(packet *packets, u32 packetCount)
{
    u64 result = 0xffffffffffffffff;
    for (u32 packetIndex = 0; packetIndex < packetCount; packetIndex++)
    {
        packet p = *(packets + packetIndex);
        u64 value = Execute(p);
        if (value < result)
            result = value;
    }
    return result;
}

internal u64
PacketsMax(packet *packets, u32 packetCount)
{
    u64 result = 0;
    for (u32 packetIndex = 0; packetIndex < packetCount; packetIndex++)
    {
        packet p = *(packets + packetIndex);
        u64 value = Execute(p);
        if (value > result)
            result = value;
    }
    return result;
}

internal u64
PacketsGreaterThan(packet *packets)
{
    packet p1 = *packets;
    packet p2 = *(packets + 1);
    u64 value1 = Execute(p1);
    u64 value2 = Execute(p2);
    return (value1 > value2);
}

internal u64
PacketsLessThan(packet *packets)
{
    packet p1 = *packets;
    packet p2 = *(packets + 1);
    u64 value1 = Execute(p1);
    u64 value2 = Execute(p2);
    return (value1 < value2);
}

internal u64
PacketsEqualTo(packet *packets)
{
    packet p1 = *packets;
    packet p2 = *(packets + 1);
    u64 value1 = Execute(p1);
    u64 value2 = Execute(p2);
    return (value1 == value2);
}

internal u64
Execute(packet p)
{
    u64 result = 0;

    switch (p.TypeId)
    {
        case 0: result = PacketsSum(p.Subpackets, p.SubpacketCount); break;
        case 1: result = PacketsProduct(p.Subpackets, p.SubpacketCount); break;
        case 2: result = PacketsMin(p.Subpackets, p.SubpacketCount); break;
        case 3: result = PacketsMax(p.Subpackets, p.SubpacketCount); break;
        case 4: result = p.Value; break;
        case 5: result = PacketsGreaterThan(p.Subpackets); break;
        case 6: result = PacketsLessThan(p.Subpackets); break;
        case 7: result = PacketsEqualTo(p.Subpackets); break;
        InvalidDefaultCase;
    };

    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day16-input.txt");
    parse_result parseResult = Parse(file);

    // _versionSum is calculated globally, because why not.

    return _versionSum;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day16-input.txt");
    parse_result parseResult = Parse(file);
    packets_info packetsInfo = parseResult.PacketsInfo;
    u64 result = 0;

    Assert(packetsInfo.PacketCount == 1);

    packet p = *packetsInfo.Packets;
    result = Execute(p);

    return result;
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u64 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 16 -\n");
    DebugLog("Result Part 1: %d (%d ms, %llu cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %llu (%d ms, %llu cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}