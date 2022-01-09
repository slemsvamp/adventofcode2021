#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct coordinate
{
    u16 X;
    u16 Y;
};

struct amphipod
{
    coordinate Position;
    char Type;
    u64 Cost;
};

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\temp.txt");

// #############
// #...........#
// ###D#B#A#C###
//   #B#D#A#C#
//   #########

/*
    A 7
    A 7
    C 500
    C 500
    D 9000
    B 20
    D 7000
    B 30
    B 50
    A 3
    A 3
*/


    return 0;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\temp.txt");

// #############
// #...........#
// ###D#B#A#C###
//   #D#C#B#A#
//   #D#B#A#C#
//   #B#D#A#C#
//   #########

    return 0;
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

    DebugLog("- Day 23 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}