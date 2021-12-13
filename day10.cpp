#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

internal u64
GetCorruptionPoints(char character)
{
    switch (character)
    {
        case ')': return 3;
        case ']': return 57;
        case '}': return 1197;
        case '>': return 25137;
        default:
            return 0;
    };
}

internal u64
GetCompletionPoints(char character)
{
    switch (character)
    {
        case ')': return 1;
        case ']': return 2;
        case '}': return 3;
        case '>': return 4;
        default:
            return 0;
    };
}

internal b8
IsStartingCharacter(char character)
{
    return character == '(' || character == '[' || character == '{' || character == '<';
}

internal b8
IsEndingCharacter(char character)
{
    return character == ')' || character == ']' || character == '}' || character == '>';
}

internal b8
IsMatchingPair(char start, char end)
{
    switch (start)
    {
        case '(': return end == ')';
        case '[': return end == ']';
        case '{': return end == '}';
        case '<': return end == '>';
        default:
            return 0;
    }
}

internal char
GetMatchingClosingCharacter(char character)
{
    switch (character)
    {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        case '<': return '>';
        default:
            return 0;
    }
}

struct corruption_result
{
    char Character;
    char *StartingCharacters;
    u8 StartIndex;
};

internal corruption_result
CheckSimpleCorruption(char *line)
{
    u32 depth = 0;

    char *playhead = line;
    char *startStack = (char *)calloc(256, 1);
    u8 startStackIndex = 0;

    while (*playhead)
    {
        if (!startStackIndex)
            *(startStack + startStackIndex++) = *playhead;
        else
        {
            if (IsStartingCharacter(*playhead))
                *(startStack + startStackIndex++) = *playhead;
            else if (IsEndingCharacter(*playhead) && !IsMatchingPair(*(startStack + --startStackIndex), *playhead))
            {
                free(startStack);
                return { *playhead, 0, 0 };
            }
        }

        playhead++;
    }

    return { 0, startStack, startStackIndex };
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day10-input.txt");

    char *line = strtok(file.Data, "\r\n");
    u32 sum = 0;

    while (line)
    {
        corruption_result corruption = CheckSimpleCorruption(line);

        sum += GetCorruptionPoints(corruption.Character);

        line = strtok(NULL, "\r\n");
    }

    return sum;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day10-input.txt");

    char *line = strtok(file.Data, "\r\n");
    u64 *scores = (u64 *)calloc(110, sizeof(u64));
    u32 scoreIndex = 0;

    while (line)
    {
        u64 score = 0;
        corruption_result corruption = CheckSimpleCorruption(line);

        if (corruption.StartingCharacters)
        {
            while (corruption.StartIndex--)
            {
                char closingCharacter = GetMatchingClosingCharacter(*(corruption.StartingCharacters + corruption.StartIndex));
                score = score * 5 + GetCompletionPoints(closingCharacter);
            }
            
            *(scores + scoreIndex++) = score;
        }
        
        free(corruption.StartingCharacters);
        line = strtok(NULL, "\r\n");
    }

    QS_Sort(scores, 0, --scoreIndex);

    u32 middleIndex = scoreIndex / 2;

    return *(scores + middleIndex);
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u64 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u64 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 10 -\n");
    DebugLog("Result Part 1: %lld (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}