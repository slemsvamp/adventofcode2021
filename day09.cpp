#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct size
{
    u32 Height;
    u32 Width;
};

struct vent_map
{
    size Size;
    u8 *Map;
};

internal size
GetSizeOfMap(file_data file)
{
    size result = {};

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *lines = strtok(buffer, "\r\n");
    while (lines != NULL)
    {
        if (!result.Width)
            result.Width = strlen(lines);
        result.Height++;
        lines = strtok(NULL, "\r\n");
    }

    free(buffer);
    return result;
}

internal vent_map
Parse(file_data file)
{
    vent_map result = {};

    size mapSize = GetSizeOfMap(file);

    result.Size = mapSize;
    result.Map = (u8 *)malloc(sizeof(u8) * mapSize.Height * mapSize.Width);
    
    u32 height = 0;
    char *lines = strtok(file.Data, "\r\n");
    while (lines != NULL)
    {
        u8 *mapAddress = (result.Map + height * mapSize.Width);
        for (u32 charIndex = 0; charIndex < strlen(lines); charIndex++)
            *(mapAddress + charIndex) = *(lines + charIndex) - '0';
        height++;
        lines = strtok(NULL, "\r\n");
    }

    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day09-input.txt");
    vent_map ventMap = Parse(file);
    size mapSize = ventMap.Size;

    u32 sum = 0;

    for (u32 y = 0; y < mapSize.Height; y++)
        for (u32 x = 0; x < mapSize.Width; x++)
        {
            u32 index = y * mapSize.Width + x;
            s32 upIndex = index - mapSize.Width;
            s32 downIndex = index + mapSize.Width;
            s32 leftIndex = index - 1;
            s32 rightIndex = index + 1;

            u32 higher = 0;
            u32 possible = 0;

            if (upIndex >= 0)
            {
                possible++;
                higher += *(ventMap.Map + upIndex) > *(ventMap.Map + index) ? 1 : 0;
            }
            if (downIndex <= mapSize.Width * mapSize.Height - 1)
            {
                possible++;
                higher += *(ventMap.Map + downIndex) > *(ventMap.Map + index) ? 1 : 0;
            }
            if (leftIndex >= 0)
            {
                possible++;
                higher += *(ventMap.Map + leftIndex) > *(ventMap.Map + index) ? 1 : 0;
            }
            if (rightIndex <= mapSize.Width * mapSize.Height - 1)
            {
                possible++;
                higher += *(ventMap.Map + rightIndex) > *(ventMap.Map + index) ? 1 : 0;
            }

            if (higher == possible)
                sum += *(ventMap.Map + index) + 1;
        }

    return sum;
}

internal void
Explore(u32 index, u8 *closed, vent_map ventMap, u32 *sum)
{
    size bounds = ventMap.Size;

    if (index < 0 || index > bounds.Height * bounds.Width - 1)
        return;

    if (*(closed + index))
        return;

    u8 *map = ventMap.Map;
    u8 value = *(map + index);

    *(closed + index) = 1;
    
    if (value == 9)
        return;

    (*sum)++;

    s32 y = index / bounds.Width;
    s32 x = index % bounds.Width;

    if (y > 0)
    {
        s32 upIndex = index - bounds.Width;
        Explore(upIndex, closed, ventMap, sum);
    }
    if (y < bounds.Height - 1)
    {
        s32 downIndex = index + bounds.Width;
        Explore(downIndex, closed, ventMap, sum);
    }
    if (x > 0)
    {
        s32 leftIndex = index - 1;
        Explore(leftIndex, closed, ventMap, sum);
    }
    if (x < bounds.Width - 1)
    {
        s32 rightIndex = index + 1;
        Explore(rightIndex, closed, ventMap, sum);
    }
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day09-input.txt");
    vent_map ventMap = Parse(file);
    size mapSize = ventMap.Size;

    u8 *closed = (u8 *)calloc(mapSize.Height * mapSize.Width, 1);

    u32 *basins = (u32 *)calloc(1000, sizeof(u32));
    u32 basinIndex = 0;

    for (u32 y = 0; y < mapSize.Height; y++)
        for (u32 x = 0; x < mapSize.Width; x++)
        {
            u32 sum = 0;
            u32 index = y * mapSize.Width + x;

            if (*(closed + index))
                continue;
            
            Explore(index, closed, ventMap, &sum);

            Assert(basinIndex < 1000);
            
            if (sum > 0)
                *(basins + basinIndex++) = sum;
        }
    
    QS_Sort(basins, 0, basinIndex - 1);

    u32 result = *(basins + basinIndex - 1) * *(basins + basinIndex - 2) * *(basins + basinIndex - 3);

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

    u32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 09 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}