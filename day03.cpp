#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"

enum bit_criteria
{
    bit_criteria_least = 0,
    bit_criteria_most = 1
};

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day03-input.txt");
    char *playhead = file.Data;

    char *fileDataIndex = file.Data;
    while (*(fileDataIndex) != '\r')
        fileDataIndex++;

    u32 length = (u32)(fileDataIndex - file.Data);
    u32 *ones = (u32 *)calloc(1, sizeof(u32) * length);
    u32 *zeroes = (u32 *)calloc(1, sizeof(u32) * length);

    while (playhead != NULL && (playhead - file.Data) < (u32)file.Size)
    {
        u32 index = (playhead - file.Data) % (length + 2);
        if (*playhead == '\r' && *(playhead+1) == '\n')
            playhead++;
        else if (*playhead == '1')
            (*(ones + index))++;
        else
            (*(zeroes + index))++;
        playhead++;
    }

    u32 gammaRate = 0;
    u32 epsilonRate = 0;

    for (u32 bitIndex = 0; bitIndex < length; bitIndex++)
        if (*(ones + bitIndex) > *(zeroes + bitIndex))
            gammaRate += 1 << (length - bitIndex - 1);
        else
            epsilonRate += 1 << (length - bitIndex - 1);

    u64 powerConsumption = gammaRate * epsilonRate;

    return length;
}

internal char
NextSoughtValue(char *playhead, u32 bitIndex, u32 length, bit_criteria bitCriteria, u32 size)
{
    char *initial = playhead;

    u32 *ones = (u32 *)calloc(1, sizeof(u32) * length);
    u32 *zeroes = (u32 *)calloc(1, sizeof(u32) * length);

    while (playhead != NULL && (playhead - initial) < size)
    {
        if (*playhead == NULL)
        {
            playhead += length + 2;
            continue;
        }

        u32 index = (playhead - initial) % (length + 2);
        if (*playhead == '\r' && *(playhead + 1) == '\n')
            playhead++;
        else if (*playhead == '1')
            (*(ones + index))++;
        else
            (*(zeroes + index))++;
        playhead++;
    }

    u32 result = 0;
    if (bitCriteria == bit_criteria_most)
        result = *(ones + bitIndex) >= *(zeroes + bitIndex);
    else if (bitCriteria == bit_criteria_least)
        result = *(ones + bitIndex) < *(zeroes + bitIndex);

    playhead = initial;

    free(ones);
    free(zeroes);

    return (char)(result + '0');
}

internal u32
ClearFilter(char *playhead, u32 bitIndex, u32 length, char keep, u32 size)
{
    char *initial = playhead;

    u32 result = 0;
    while (playhead != NULL && (playhead - initial) < size)
    {
        if (*(playhead + bitIndex) != keep)
            memset(playhead, 0, length);
        else
            result++;
        playhead += length + 2;
    }

    playhead = initial;

    return result;
}

internal u64
Part2(u32 length)
{
    file_data file = ReadToEndOfFile("input\\day03-input.txt");
    u32 fileSize = (u32)file.Size;

    char *buffer = (char *)malloc(file.Size);

    memcpy(buffer, file.Data, file.Size);

    u32 remaining = 0xffffffff;
    char bit = 0;

    for (u32 bitIndex = 0; bitIndex < length && remaining > 1; bitIndex++)
    {
        bit = NextSoughtValue(buffer, bitIndex, length, bit_criteria_most, fileSize);
        remaining = ClearFilter(buffer, bitIndex, length, bit, fileSize);
        Assert(remaining > 0);
    }
    
    u32 oxygenGeneratorRating = 0;
    for (char *playhead = buffer; playhead - buffer < (u32)file.Size; playhead += length + 2)
        if (*playhead != 0)
        {
            for (char *bitPlayhead = playhead; (bitPlayhead - playhead) < length; bitPlayhead++)
                oxygenGeneratorRating += (*bitPlayhead - '0') << (length - (bitPlayhead - playhead) - 1);
            break;
        }


    memcpy(buffer, file.Data, file.Size);

    remaining = 0xffffffff;

    for (u32 bitIndex = 0; bitIndex < length && remaining > 1; bitIndex++)
    {
        bit = NextSoughtValue(buffer, bitIndex, length, bit_criteria_least, fileSize);
        remaining = ClearFilter(buffer, bitIndex, length, bit, fileSize);
        Assert(remaining > 0);
    }

    u32 scrubberRating = 0;

    for (char *playhead = buffer; playhead - buffer < (u32)file.Size; playhead += length + 2)
        if (*playhead != 0)
        {
            for (char *bitPlayhead = playhead; (bitPlayhead - playhead) < length; bitPlayhead++)
                scrubberRating += (*bitPlayhead - '0') << (length - (bitPlayhead - playhead) - 1);
            break;
        }

    u64 lifeSupportRating = oxygenGeneratorRating * scrubberRating;
 
    free(buffer);

    return lifeSupportRating;
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u64 resultPart2 = Part2(resultPart1);

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 03 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}