#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct parse_result
{
    u32 *Values;
    u32 Count;
};

internal u32
CountValues(file_data file)
{
    char *playhead = file.Data;
    u32 commas = 0;
    while ((playhead - file.Data) < file.Size)
    {
        if (*playhead == ',')
            commas++;
        playhead++;
    }
    u32 result = commas + 1;
    return result;
}

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    result.Count = CountValues(file);
    result.Values = (u32 *)malloc(sizeof(u32) * result.Count);
    
    u32 valueIndex = 0;
    char *number = strtok(file.Data, ",");

    while (number != NULL)
    {
        *(result.Values + valueIndex++) = atoi(number);
        number = strtok(NULL, ",");
    }

    return result;
}

internal u32
CalculateTotalCost(u32 halfWay, u32 *values, u32 count, b8 constantRate)
{
    u64 result = 0;

    u32 *playhead = values;
    while (*playhead < halfWay)
    {
        u32 distance = halfWay - *playhead;
        if (constantRate)
            result += distance;
        else
            result += distance * (distance + 1) / 2;
        playhead++;
    }

    while (playhead - values < count)
    {
        u32 distance = *playhead - halfWay;
        if (constantRate)
            result += distance;
        else
            result += distance * (distance + 1) / 2;
        playhead++;
    }

    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day07-input1.txt");
    parse_result parseResult = Parse(file);

    QS_Sort(parseResult.Values, 0, parseResult.Count - 1);

    u32 lastCost = CalculateTotalCost(0, parseResult.Values, parseResult.Count, true);

    for (u32 index = 0; index < parseResult.Count - 1; index++)
    {
        u32 cost = CalculateTotalCost(index, parseResult.Values, parseResult.Count, true);

        if (cost > lastCost)
            return lastCost;
        
        lastCost = cost;
    }
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day07-input1.txt");
    parse_result parseResult = Parse(file);

    QS_Sort(parseResult.Values, 0, parseResult.Count - 1);

    u32 lastCost = CalculateTotalCost(0, parseResult.Values, parseResult.Count, false);

    for (u32 index = 0; index < parseResult.Count - 1; index++)
    {
        u32 cost = CalculateTotalCost(index, parseResult.Values, parseResult.Count, false);

        if (cost > lastCost)
            return lastCost;
        
        lastCost = cost;
    }
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 07 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}