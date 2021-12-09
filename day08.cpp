#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct wire_information
{
    char *Clues;
    char *Digits;
};

// wires are linked to segment, they can individually be on

struct parse_result
{
    wire_information *Information;
    u32 Count;
};

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *playhead = buffer;
    char *line = strtok(playhead, "\r\n");

    u32 *sizes = (u32 *)calloc(1000, sizeof(u32));

    u32 count = 0;
    while (line)
    {
        // we remove 61, i'm only interested in the part that varies in length
        // so we're removing all that is infront of the pipe sign and one space after.
        *(sizes + count) = strlen(line) - 61;
        count++;
        
        Assert(count < 1000);

        line = strtok(NULL, "\r\n");
    }

    result.Count = count;
    result.Information = (wire_information *)calloc(count, sizeof(wire_information));

    playhead = file.Data;
    line = strtok(playhead, "\r\n");
    u32 informationIndex = 0;

    while (line)
    {
        u32 digitLength = *(sizes + informationIndex);
        
        wire_information wireInformation = {};
        wireInformation.Clues = (char *)malloc(59);
        wireInformation.Digits = (char *)malloc(digitLength + 1);
        
        memcpy(wireInformation.Clues, line, 58);
        *(wireInformation.Clues + 58) = '\0';

        memcpy(wireInformation.Digits, line + 61, digitLength);
        *(wireInformation.Digits + digitLength) = '\0';

        *(result.Information + informationIndex) = wireInformation;

        informationIndex++;

        line = strtok(NULL, "\r\n");
    }

    return result;
}

internal u32
CountAppearance(u32 segmentCount, parse_result parseResult)
{
    u32 result = 0;
    for (u32 informationIndex = 0; informationIndex < parseResult.Count; informationIndex++)
    {
        wire_information information = *(parseResult.Information + informationIndex);

        u32 length = strlen(information.Digits);
        char *buffer = (char *)malloc(length + 1);
        memcpy(buffer, information.Digits, length);
        *(buffer + length) = '\0';

        char *segment = strtok(buffer, " ");
        while (segment)
        {
            if (strlen(segment) == segmentCount)
                result++;
            segment = strtok(NULL, " ");
        }

        free(buffer);
    }
    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day08-input1.txt");
    parse_result parseResult = Parse(file);

    u32 oneCount = CountAppearance(2, parseResult);
    u32 fourCount = CountAppearance(4, parseResult);
    u32 sevenCount = CountAppearance(3, parseResult);
    u32 eightCount = CountAppearance(7, parseResult);

    u32 result = oneCount + fourCount + sevenCount + eightCount;

    return result;
}

internal char**
GetDigitsByCount(u32 segmentCount, wire_information *information)
{
    char **result = (char **)calloc(3, sizeof(size_t));

    u32 length = strlen(information->Clues);
    char *buffer = (char *)malloc(length + 1);
    memcpy(buffer, information->Clues, length);
    *(buffer + length) = '\0';

    u32 segmentIndex = 0;

    char *segment = strtok(buffer, " ");
    while (segment)
    {
        if (strlen(segment) == segmentCount)
        {
            char *digits = (char *)calloc(segmentCount + 1, 1);
            memcpy(digits, segment, segmentCount);
            *(result + segmentIndex) = digits;
            segmentIndex++;
        }

        segment = strtok(NULL, " ");
    }

    free(buffer);
    return result;
}

internal char*
GetSegmentsByCount(u32 segmentCount, wire_information *information)
{
    char *result = (char *)calloc(segmentCount, 1);

    u32 length = strlen(information->Clues);
    char *buffer = (char *)malloc(length + 1);
    memcpy(buffer, information->Clues, length);
    *(buffer + length) = '\0';

    char *segment = strtok(buffer, " ");
    while (segment)
    {
        if (strlen(segment) == segmentCount)
        {
            memcpy(result, segment, segmentCount);
            free(buffer);
            return result;
        }
        segment = strtok(NULL, " ");
    }

    return NULL;
}

internal char*
SubtractSegments(char *a, char *b)
{
    u32 aLength = strlen(a);
    u32 bLength = strlen(b);
    
    char *buffer = (char *)malloc(aLength + 1);
    memcpy(buffer, a, aLength);
    *(buffer + aLength) = '\0';

    u32 segmentIndex = 0;
    char *result = (char *)calloc(aLength + 1, 1);

    for (u32 indexA = 0; indexA < aLength; indexA++)
    {
        char filter = *(buffer + indexA);
        b8 found = false;
        for (u32 indexB = 0; indexB < bLength; indexB++)
            if (filter == *(b + indexB))
            {
                found = true;
                break;
            }
        
        if (!found)
        {
            *(result + segmentIndex) = filter;
            segmentIndex++;
        }
    }

    free(buffer);

    return result;
}

internal u32
GetValueFromWireInformation(char **segmentInformation, wire_information *information)
{
    u32 length = strlen(information->Digits);
    char *buffer = (char *)malloc(length + 1);
    memcpy(buffer, information->Digits, length);
    *(buffer + length) = '\0';

    char *segments = strtok(buffer, " ");

    char *digitsInText = (char *)calloc(5, 1);
    u32 textIndex = 0;

    while (segments)
    {
        for (u32 digitIndex = 0; digitIndex < 10; digitIndex++)
        {
            char *digitSegments = *(segmentInformation + digitIndex);
            
            if (strlen(digitSegments) == strlen(segments) && strlen(SubtractSegments(digitSegments, segments)) == 0)
            {
                *(digitsInText + textIndex) = '0' + digitIndex;
                textIndex++;
            }
        }
        segments = strtok(NULL, " ");
    }

    free(buffer);

    u32 result = atoi(digitsInText);
    return result;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day08-input1.txt");
    parse_result parseResult = Parse(file);

    u64 sum = 0;

    for (u32 informationIndex = 0; informationIndex < parseResult.Count; informationIndex++)
    {
        wire_information *wireInformation = (parseResult.Information + informationIndex);
        
        char **segmentInformation = (char **)calloc(10, sizeof(size_t));

        *(segmentInformation + 1) = GetSegmentsByCount(2, wireInformation);
        *(segmentInformation + 4) = GetSegmentsByCount(4, wireInformation);
        *(segmentInformation + 7) = GetSegmentsByCount(3, wireInformation);
        *(segmentInformation + 8) = GetSegmentsByCount(7, wireInformation);
        
        char *topSegment = SubtractSegments(*(segmentInformation + 7), *(segmentInformation + 1));
        char *topLeftAndMiddleSegment = SubtractSegments(*(segmentInformation + 4), *(segmentInformation + 1));

        char **sixSegmentDigits = GetDigitsByCount(6, wireInformation); // 0, 6, 9

        char *middleSegment;
        char *topLeftSegment;

        for (u32 digitIndex = 0; digitIndex < 3; digitIndex++)
        {
            char *candidateMiddleSegment = SubtractSegments(topLeftAndMiddleSegment, *(sixSegmentDigits + digitIndex));
            if (strlen(candidateMiddleSegment) == 1)
            {
                *(segmentInformation + 0) = *(sixSegmentDigits + digitIndex);
                middleSegment = candidateMiddleSegment;
                topLeftSegment = SubtractSegments(topLeftAndMiddleSegment, middleSegment);
                break;
            }
        }

        b8 assignedFiveOrSix = false;

        for (u32 digitIndex = 0; digitIndex < 3; digitIndex++)
        {
            char *digit = *(sixSegmentDigits + digitIndex);
            if (digit == *(segmentInformation + 0))
                continue;

            if (assignedFiveOrSix)
            {
                if (*(segmentInformation + 6))
                    *(segmentInformation + 9) = digit;
                else
                    *(segmentInformation + 6) = digit;
                continue;
            }

            assignedFiveOrSix = true;

            if (strlen(SubtractSegments(digit, *(segmentInformation + 1))) == 5)
                *(segmentInformation + 6) = digit;
            else
                *(segmentInformation + 9) = digit;
        }

        char **fiveSegmentDigits = GetDigitsByCount(5, wireInformation); // 2, 3, 5

        for (u32 digitIndex = 0; digitIndex < 3; digitIndex++)
        {
            char *digit = *(fiveSegmentDigits + digitIndex);

            char *topLeftSegmentCheck = SubtractSegments(topLeftSegment, digit);

            if (strlen(topLeftSegmentCheck) == 0)
                *(segmentInformation + 5) = digit;
            else
            {
                u32 lengthTwoOrThree = strlen(SubtractSegments(digit, *(segmentInformation + 1)));
                if (lengthTwoOrThree == 3)
                    *(segmentInformation + 3) = digit;
                else
                    *(segmentInformation + 2) = digit;
            }
        }

        // by now we have 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
        for (u32 assertIndex = 0; assertIndex < 10; assertIndex++)
            Assert(*(segmentInformation + assertIndex));

        u32 value = GetValueFromWireInformation(segmentInformation, wireInformation);

        sum += value;
    }

    return sum;
}

u32
main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u64 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 08 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}