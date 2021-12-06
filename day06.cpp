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

internal u64
SimulateDays(u32 days, u32 *startingValues, u32 count)
{
    u64 *state = (u64 *)calloc(9, sizeof(u64));

    for (u32 valueIndex = 0; valueIndex < count; valueIndex++)
        (*(state + *(startingValues + valueIndex)))++;
    
    for (u32 dayIndex = 0; dayIndex < days; dayIndex++)
    {
        u64 newFish = 0;

        for (u32 duplicationCountIndex = 0; duplicationCountIndex < 9; duplicationCountIndex++)
        {
            u64 *statePointer = state + duplicationCountIndex;
            u64 fishCount = *statePointer;

            if (duplicationCountIndex == 0)
                newFish = fishCount;
            else
                *(statePointer - 1) += fishCount;

            *statePointer -= fishCount;
        }

        *(state + 6) += newFish;
        *(state + 8) += newFish;
    }

    u64 result = 0;
    for (u32 duplicationCountIndex = 0; duplicationCountIndex < 9; duplicationCountIndex++)
        result += *(state + duplicationCountIndex);

    free(state);

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

internal void
Part1()
{
    const u32 simulateDays = 80;

    file_data file = ReadToEndOfFile("input\\day06-input1.txt");
    parse_result parseResult = Parse(file);
    u64 numberOfFish = SimulateDays(simulateDays, parseResult.Values, parseResult.Count);

    DebugLog("Result Part 1: %lld\n", numberOfFish);
}

internal void
Part2()
{
    const u32 simulateDays = 256;

    file_data file = ReadToEndOfFile("input\\day06-input1.txt");
    parse_result parseResult = Parse(file);
    u64 numberOfFish = SimulateDays(simulateDays, parseResult.Values, parseResult.Count);

    DebugLog("Result Part 2: %lld\n", numberOfFish);
}

u32
main(s32 argumentCount, char *arguments[])
{
    Part1();
    Part2();

    return 0;
}