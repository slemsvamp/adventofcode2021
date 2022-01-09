#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

u32 _deterministicHundredSidedDieValue = 1;

struct parse_result
{
    u32 PlayerOnePosition;
    u32 PlayerTwoPosition;
};

internal parse_result
Parse(file_data file)
{
    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);

    parse_result result = {};

    while (line)
    {
        if (strstr(line, "Player 1 starting position:"))
            result.PlayerOnePosition = atoi(line + 28);
        if (strstr(line, "Player 2 starting position:"))
            result.PlayerTwoPosition = atoi(line + 28);
        line = strtok(NULL, NEW_LINE);
    }

    return result;
}

internal u32
Roll()
{
    u32 result = _deterministicHundredSidedDieValue++;
    if (_deterministicHundredSidedDieValue > 100)
        _deterministicHundredSidedDieValue = 1;
    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day21-input.txt");
    parse_result parseResult = Parse(file);

    b8 playerOneRolls = true;

    u32 playerOneScore = 0;
    u32 playerTwoScore = 0;

    u32 rolls = 0;

    for (;;)
    {
        u32 roll = Roll() + Roll() + Roll();
        rolls += 3;
        if (playerOneRolls)
        {
            parseResult.PlayerOnePosition += roll;
            while (parseResult.PlayerOnePosition > 10)
                parseResult.PlayerOnePosition -= 10;
            playerOneScore += parseResult.PlayerOnePosition;
        }
        else
        {
            parseResult.PlayerTwoPosition += roll;
            while (parseResult.PlayerTwoPosition > 10)
                parseResult.PlayerTwoPosition -= 10;
            playerTwoScore += parseResult.PlayerTwoPosition;
        }

        if (playerOneScore >= 1000)
            return playerTwoScore * rolls;
        if (playerTwoScore >= 1000)
            return playerOneScore * rolls;

        playerOneRolls = !playerOneRolls;
    }

    return 0;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day21-input.txt");
    parse_result parseResult = Parse(file);

    


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

    DebugLog("- Day 21 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}