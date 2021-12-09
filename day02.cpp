#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

internal s32
Part1()
{
    file_data data = ReadToEndOfFile("input\\day02-input1.txt");

    s32 depth = 0;
    s32 horizontal = 0;

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        if (strstr(lines, "forward"))
            horizontal += atoi(lines + 8);
        else if (strstr(lines, "up"))
            depth -= atoi(lines + 3);
        else
            depth += atoi(lines + 5);

        lines = strtok(NULL, "\r\n");
    }

    return depth * horizontal;
}

internal s32
Part2()
{
    file_data data = ReadToEndOfFile("input\\day02-input1.txt");

    s32 depth = 0;
    s32 horizontal = 0;
    s32 aim = 0;

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        if (strstr(lines, "forward"))
        {
            s32 move = atoi(lines + 8);
            horizontal += move;
            depth += aim * move;
        }
        else if (strstr(lines, "up"))
            aim -= atoi(lines + 3);
        else
            aim += atoi(lines + 5);

        lines = strtok(NULL, "\r\n");
    }

    return depth * horizontal;
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

    DebugLog("- Day 02 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}