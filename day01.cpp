#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

internal u32
Part1()
{
    u32 depthCompare = 0;
    u32 increases = 0; 
    
    file_data data = ReadToEndOfFile("input\\day01-input1.txt");

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        u32 depth = atoi(lines);
        if (depthCompare && depth > depthCompare)
            increases++;
        depthCompare = depth;
        lines = strtok(NULL, "\r\n");
    }

    return increases;
}

internal u32
Part2()
{
    u32 depths[3] = {0, 0, 0};
    u32 depthCompare = 0;
    u32 linePlayhead = 0;
    u32 increases = 0;

    file_data data = ReadToEndOfFile("input\\day01-input1.txt");

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        u32 depth = atoi(lines);
        u32 resetIndex = linePlayhead % 3;
        
        for (u32 v = 0; v <= __min(linePlayhead, 2); v++)
            depths[v] = resetIndex == v ? depth : depths[v] + depth;

        if (linePlayhead >= 2)
        {
            u32 classIndex = (linePlayhead + 1) % 3;
            if (depthCompare && depths[classIndex] > depthCompare)
                increases++;
            depthCompare = depths[classIndex];
        }
        
        linePlayhead++;
        lines = strtok(NULL, "\r\n");
    }

    return increases;
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    s32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    s32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 01 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}