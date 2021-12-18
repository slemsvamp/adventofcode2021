#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";
const u64 MEMORY_ARRAY_SIZE = 128;
const u64 MAX_AMPLITUDE = 3; // if we would have numbers higher than 99999 (5 characters)
                             // like 100000 (6 characters) we would need to have MAX_AMPLITUDE = 6

struct split_data
{
    char *Data;
    u32 Value;
};

struct explode_data
{
    char *Data;
    u32 LeftValue;
    u32 RightValue;
    u32 Length;
};

internal void
Explode(char *buffer, explode_data ed)
{
    /*
        To explode a pair, the pair's left value is added to the
        first regular number to the left of the exploding pair
        (if any), and the pair's right value is added to the
        first regular number to the right of the exploding pair
        (if any). Exploding pairs will always consist of two
        regular numbers. Then, the entire exploding pair is
        replaced with the regular number 0.
    */

    char *temp = (char *)calloc(MEMORY_ARRAY_SIZE, sizeof(char));

    char *searchLeft = ed.Data;
    b8 foundLeftNumber = 0;
    char *searchLeft_RightBound = 0;
    while (--searchLeft >= buffer)
        if (*searchLeft >= '0' && *searchLeft <= '9')
        {
            foundLeftNumber = 1;
            searchLeft_RightBound = searchLeft; // saves me some work
            while (searchLeft - 1 >= buffer && *(searchLeft - 1) >= '0' && *(searchLeft - 1) <= '9')
                searchLeft--;
            break;
        }

    u32 leftNumber = 0;
    if (foundLeftNumber)
    {
        leftNumber = atoi(searchLeft);
        leftNumber += ed.LeftValue;
    }

    char *searchRight = ed.Data + ed.Length - 1;
    b8 foundRightNumber = 0;
    char *searchRight_RightBound = 0;
    while (*(++searchRight))
        if (*searchRight >= '0' && *searchRight <= '9')
        {
            foundRightNumber = 1;
            char *searchRightBound = searchRight;
            while (*(searchRightBound + 1) && *(searchRightBound + 1) >= '0' && *(searchRightBound + 1) <= '9')
                searchRightBound++;
            searchRight_RightBound = searchRightBound; // saves me some work
            break;
        }

    u32 rightNumber = 0;
    if (foundRightNumber)
    {
        rightNumber = atoi(searchRight);
        rightNumber += ed.RightValue;
    }

    if (foundLeftNumber)
    {
        memcpy(temp, buffer, searchLeft - buffer);
        char *leftNumberString = (char *)calloc(MAX_AMPLITUDE + 1, sizeof(char));
        itoa(leftNumber, leftNumberString, 10);
        strcat(temp, leftNumberString);

        u32 tempLength = strlen(temp);
        u32 copyLength = ed.Data - searchLeft_RightBound - 1;

        for (u32 copyIndex = 0; copyIndex < copyLength; copyIndex++)
            *(temp + tempLength + copyIndex) = *(searchLeft_RightBound + 1 + copyIndex);

        free(leftNumberString);
    }
    else
    {
        if (ed.Data > buffer)
            memcpy(temp, buffer, ed.Data - buffer);
    }

    strcat(temp, "0");

    u32 bufferLength = strlen(buffer);
    if (foundRightNumber)
    {
        u32 tempLength = strlen(temp);
        u32 copyLength = searchRight - (ed.Data + ed.Length);

        for (u32 copyIndex = 0; copyIndex < copyLength; copyIndex++)
            *(temp + tempLength + copyIndex) = *(ed.Data + ed.Length + copyIndex);

        tempLength += copyLength;

        char *rightNumberString = (char *)calloc(MAX_AMPLITUDE + 1, sizeof(char));
        itoa(rightNumber, rightNumberString, 10);
        strcat(temp, rightNumberString);

        tempLength += strlen(rightNumberString);

        char *copyUntilEnd = searchRight_RightBound + 1;
        for (u32 copyIndex = 0; *(copyUntilEnd + copyIndex); copyIndex++)
            *(temp + tempLength + copyIndex) = *(copyUntilEnd + copyIndex);

        free(rightNumberString);
    }
    else
        memcpy(temp + strlen(temp), ed.Data + ed.Length, bufferLength - (ed.Data + ed.Length - buffer));

    memcpy(buffer, temp, MEMORY_ARRAY_SIZE);
    free(temp);
}

internal void
Split(char *buffer, split_data sd)
{
    /*
        To split a regular number, replace it with a pair;
        the left element of the pair should be the regular
        number divided by two and rounded down, while the
        right element of the pair should be the regular
        number divided by two and rounded up. For example,
        10 becomes [5,5], 11 becomes [5,6], 12 becomes [6,6],
        and so on.
    */
    char *temp = (char *)calloc(MEMORY_ARRAY_SIZE, sizeof(char));

    char *valueNumberString = (char *)calloc(MAX_AMPLITUDE + 1, sizeof(char));
    char *leftNumberString = (char *)calloc(MAX_AMPLITUDE + 1, sizeof(char));
    char *rightNumberString = (char *)calloc(MAX_AMPLITUDE + 1, sizeof(char));

    u32 valueNumber = sd.Value;
    u32 leftNumber = sd.Value / 2;
    u32 rightNumber = leftNumber + (sd.Value % 2);

    itoa(valueNumber, valueNumberString, 10);
    itoa(leftNumber, leftNumberString, 10);
    itoa(rightNumber, rightNumberString, 10);

    memcpy(temp, buffer, sd.Data - buffer);
    strcat(temp, "[");
    strcat(temp, leftNumberString);
    strcat(temp, ",");
    strcat(temp, rightNumberString);
    strcat(temp, "]");

    u32 tempLength = strlen(temp);
    char *untilTheEnd = sd.Data + strlen(valueNumberString);
    char *tempPointer = temp + tempLength;

    while (*untilTheEnd)
        *(tempPointer++) = *untilTheEnd++;

    free(valueNumberString);
    free(leftNumberString);
    free(rightNumberString);

    memcpy(buffer, temp, MEMORY_ARRAY_SIZE);
    free(temp);
}

internal u64
Magnitude(char **buffer)
{
/*
    The magnitude of a pair is 3 times the magnitude of its
    left element plus 2 times the magnitude of its right element.
    The magnitude of a regular number is just that number.

    For example, the magnitude of [9,1] is 3*9 + 2*1 = 29;
    the magnitude of [1,9] is 3*1 + 2*9 = 21. Magnitude calculations
    are recursive:
        the magnitude of [[9,1],[1,9]] is 3*29 + 2*21 = 129.
*/

    u32 result = 0;

    char *playhead = *buffer;

    if (*playhead >= '0' && *playhead <= '9')
    {
        result = (u64)atoll(playhead);
        
        while (*playhead >= '0' && *playhead <= '9')
            playhead++;
        *buffer = playhead;
        return result;
    }

    u64 leftSide;

    if (*playhead == '[')
    {
        playhead++;
        leftSide = 3 * Magnitude(&playhead);
    }

    Assert(*playhead == ',');
    playhead++;

    u32 rightSide = 2 * Magnitude(&playhead);

    Assert(*playhead == ']');
    playhead++;

    result = leftSide + rightSide;

    *buffer = playhead;

    return result;
}

internal split_data
SearchSplit(char *playhead)
{
    split_data result = {};

    while (*playhead)
    {
        if (*playhead >= '0' && *playhead <= '9')
        {
            u32 value = atoi(playhead);

            if (value > 9)
            {
                result.Value = value;
                result.Data = playhead;
                return result;
            }

            while (*playhead >= '0' && *playhead <= '9')
                playhead++;
        }
        else
            playhead++;
    }

    return result;
}

internal explode_data
SearchExplode(char *playhead)
{
    explode_data result = {};

    u32 depth = 0;
    u32 left = 0;

    while (*playhead)
    {
        if (*playhead == '[')
            depth++;
        if (*playhead == ']')
            depth--;

        if (depth >= 5)
        {
            char *searchAhead = playhead + 1;

            if (*searchAhead >= '0' && *searchAhead <= '9')
            {
                u32 leftValue = 0;
                leftValue = (u32)atoi(searchAhead);

                while (*searchAhead != ',')
                    searchAhead++;
                searchAhead++;

                if (*searchAhead >= '0' && *searchAhead <= '9')
                {
                    u32 rightValue = 0;
                    rightValue = (u32)atoi(searchAhead);

                    while (*searchAhead >= '0' && *searchAhead <= '9')
                        searchAhead++;

                    Assert(*searchAhead == ']');
                    searchAhead++;

                    result.Data = playhead;
                    result.LeftValue = leftValue;
                    result.RightValue = rightValue;
                    result.Length = searchAhead - playhead;
                    return result;
                }
            }
        }

        left++;
        playhead++;
    }

    return result;
}

internal void
Add(char *buffer, char *append)
{
    char *tempBuffer = (char *)calloc(MEMORY_ARRAY_SIZE, sizeof(char));

    u32 appendLength = strlen(append);
    u32 bufferLength = strlen(buffer);

    char *playhead = tempBuffer;
    *playhead = '[';
    playhead++;
    memcpy(playhead, buffer, bufferLength);
    playhead += bufferLength;
    *playhead = ',';
    playhead++;
    memcpy(playhead, append, appendLength);
    playhead += appendLength;
    *playhead = ']';
    playhead++;
    *playhead = '\0';

    memcpy(buffer, tempBuffer, MEMORY_ARRAY_SIZE);
    free(tempBuffer);
}

internal void
ProcessLoop(char *playhead)
{
    for (;;)
    {
        explode_data ed = SearchExplode(playhead);
        if (ed.Data)
        {
            Explode(playhead, ed);
            continue;
        }

        split_data sd = SearchSplit(playhead);
        if (sd.Data)
        {
            Split(playhead, sd);
            continue;
        }

        break;
    }
}

internal u64
Part1()
{
    file_data file = ReadToEndOfFile("input\\day18-input.txt");
    char *buffer = (char *)calloc(MEMORY_ARRAY_SIZE, sizeof(char));

    char *readBuffer = (char *)malloc(file.Size + 1);
    memcpy(readBuffer, file.Data, file.Size);
    readBuffer[file.Size] = 0;

    char *line = strtok(readBuffer, NEW_LINE);
    memcpy(buffer, line, strlen(line));

    b8 quit = false;

    while (line)
    {
        line = strtok(NULL, NEW_LINE);

        if (line)
            Add(buffer, line);
        else
            quit = true;

        ProcessLoop(buffer);

        if (quit)
            break;
    }

    return Magnitude(&buffer);
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day18-input.txt");

    char **bufferArray = (char **)malloc(4096 * sizeof(char *));
    u32 arrayCount = 0;

    char *line = strtok(file.Data, NEW_LINE);

    while (line)
    {
        *(bufferArray + arrayCount++) = line;
        line = strtok(NULL, NEW_LINE);
    }

    char **array = (char **)malloc(arrayCount * sizeof(char *));
    memcpy(array, bufferArray, arrayCount * sizeof(char *));
    free(bufferArray);

    u64 maxMagnitude = 0;
    for (u32 first = 0; first < arrayCount; first++)
    {
        for (u32 second = 0; second < arrayCount; second++)
        {
            if (first == second)
                continue;

            char *temp = (char *)calloc(MEMORY_ARRAY_SIZE, sizeof(char));
            memcpy(temp, *(array + first), strlen(*(array + first)));

            Add(temp, *(array + second));
            ProcessLoop(temp);

            char *magnitudePlayhead = temp;

            u64 magnitude = Magnitude(&magnitudePlayhead);

            maxMagnitude = __max(maxMagnitude, magnitude);

            free(temp);
        }
    }

    return maxMagnitude;
}

u32 main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u64 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u64 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 18 -\n");
    DebugLog("Result Part 1: %lld (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}