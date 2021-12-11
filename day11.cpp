#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct size
{
    u32 Width;
    u32 Height;
};

struct octopi_map
{
    u8 *EnergyLevels;
    size Size;
};

s8 _dirVertical[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
s8 _dirHorizontal[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

internal octopi_map
Parse(file_data file)
{
    octopi_map result = {};

    const char *NEW_LINE = "\r\n";

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);
    size mapSize = {};

    while (line)
    {
        if (!mapSize.Width)
            mapSize.Width = strlen(line);
        mapSize.Height++;
        line = strtok(NULL, NEW_LINE);
    }

    result.Size = mapSize;

    memcpy(buffer, file.Data, file.Size);
    result.EnergyLevels = (u8 *)calloc(mapSize.Width * mapSize.Height, sizeof(u8));
    u32 energyLevelIndex = 0;

    line = strtok(buffer, NEW_LINE);
    while (line)
    {
        for (u32 lineIndex = 0; lineIndex < mapSize.Width; lineIndex++)
            *(result.EnergyLevels + energyLevelIndex++) = *(line + lineIndex) - '0';

        line = strtok(NULL, NEW_LINE);
    }

    free(buffer);
    return result;
}

struct process_result
{
    u32 FlashCount;
};

internal void
IncreaseByOne(u32 area, octopi_map *map, u32 *flashCount, u32 *newMaxEnergyLevel, u32 *newMaxEnergyLevelCount)
{
    for (u32 index = 0; index < area; index++)
    {
        u8 *energyLevel = map->EnergyLevels + index;

        if (*energyLevel < 10)
        {
            if (*energyLevel == 9)
            {
                (*flashCount)++;
                *(newMaxEnergyLevel + (*newMaxEnergyLevelCount)++) = index;
            }

            (*energyLevel)++;
        }
    }
}

internal void
Flash(octopi_map *map, u32 *flashCount, u32 *newMaxEnergyLevel, u32 *newMaxEnergyLevelCount)
{
    for (u32 octopusIndex = 0; octopusIndex < *newMaxEnergyLevelCount; octopusIndex++)
    {
        u32 index = *(newMaxEnergyLevel + octopusIndex);
        u32 y = index / map->Size.Width;
        u32 x = index % map->Size.Width;

        for (u32 directionIndex = 0; directionIndex < ArrayCount(_dirVertical); directionIndex++)
        {
            s32 checkY = y + _dirVertical[directionIndex];
            s32 checkX = x + _dirHorizontal[directionIndex];

            if (checkY < 0 || checkY >= map->Size.Height || checkX < 0 || checkX >= map->Size.Width)
                continue;

            u32 checkIndex = checkY * map->Size.Width + checkX;

            u8 *energyLevel = map->EnergyLevels + checkIndex;

            if (*energyLevel == 10)
                continue;

            if (*energyLevel == 9)
            {
                (*flashCount)++;
                *energyLevel = 10;
                *(newMaxEnergyLevel + (*newMaxEnergyLevelCount)++) = checkIndex;
            }
            else
                (*energyLevel)++;
        }
    }
}

internal void
ClearFlashedOctopi(u32 area, octopi_map *map)
{
    for (u32 index = 0; index < area; index++)
        if (*(map->EnergyLevels + index) == 10)
            *(map->EnergyLevels + index) = 0;
}

internal process_result
ProcessStep(u32 step, octopi_map *map)
{
    process_result result = {};

    u32 area = map->Size.Height * map->Size.Width;

    u32 *newMaxEnergyLevel = (u32 *)calloc(area, sizeof(u32));
    u32 newMaxEnergyLevelCount = 0;

    IncreaseByOne(area, map, &result.FlashCount, newMaxEnergyLevel, &newMaxEnergyLevelCount);

    Flash(map, &result.FlashCount, newMaxEnergyLevel, &newMaxEnergyLevelCount);

    ClearFlashedOctopi(area, map);

    free(newMaxEnergyLevel);

    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day11-input1.txt");
    octopi_map map = Parse(file);

    u32 result = 0;

    for (u32 step = 1; step <= 100; step++)
    {
        process_result processResult = ProcessStep(step, &map);
        result += processResult.FlashCount;
    }

    return result;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day11-input1.txt");

    octopi_map map = Parse(file);

    u32 area = map.Size.Height * map.Size.Width;
    u32 result = 0;

    for (u32 step = 1;; step++)
    {
        process_result processResult = ProcessStep(step, &map);
        if (processResult.FlashCount == area)
            return step;
    }

    return 0;
}

u32 main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 11 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}