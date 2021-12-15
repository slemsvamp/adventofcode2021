#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

struct size
{
    u32 Width;
    u32 Height;
};

struct parse_result
{
    u8 *RiskLevels;
    size Size;
};

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);

    while (line)
    {
        if (!result.Size.Width)
            result.Size.Width = strlen(line);
        result.Size.Height++;
        line = strtok(NULL, NEW_LINE);
    }

    memcpy(buffer, file.Data, file.Size);

    result.RiskLevels = (u8 *)malloc(result.Size.Width * result.Size.Height * sizeof(u8));

    line = strtok(buffer, NEW_LINE);
    u32 playhead = 0;
    u32 length = 0;

    while (line)
    {
        length = strlen(line);

        for (u32 memIndex = 0; memIndex < length; memIndex++)
            *(result.RiskLevels + playhead + memIndex) = *(line + memIndex) - '0';

        playhead += length;
        line = strtok(NULL, NEW_LINE);
    }

    return result;
}

s32 _verticalDirections[4] = {-1, 0, 1, 0};
s32 _horizontalDirections[4] = {0, 1, 0, -1};

internal u64
Travel(u32 *open, u32 *openCount, u32 endY, u32 endX, parse_result *cave)
{
    u32 width = cave->Size.Width;
    u32 height = cave->Size.Height;
    u32 *gScore = (u32 *)calloc(width * height, sizeof(u32));
    u32 *fScore = (u32 *)calloc(width * height, sizeof(u32));
    u32 *cameFrom = (u32 *)calloc(width * height, sizeof(u32));
    u32 endIndex = endY * width + endX;

    for (u32 scoreIndex = 0; scoreIndex < width * height; scoreIndex++)
    {
        *(gScore + scoreIndex) = 0xffffffff;
        *(fScore + scoreIndex) = 0xffffffff;
        *(cameFrom + scoreIndex) = 0xffffffff;
    }

    *(gScore) = 0;
    *(fScore) = endY + endX;

    while (*openCount > 0)
    {
        u32 lowestIndex = 0;
        u32 lowestScore = 0xffffffff;

        for (u32 openIndex = 0; openIndex < *openCount; openIndex++)
        {
            u32 fS = *(fScore + *(open + openIndex));
            if (fS < lowestScore)
            {
                lowestIndex = openIndex;
                lowestScore = fS;
            }
        }
        
        u32 index = *(open + lowestIndex);

        for (u32 openIndex = lowestIndex; openIndex < *openCount - 1; openIndex++)
            *(open + openIndex) = *(open + openIndex + 1);

        (*openCount)--;

        if (index == endIndex)
        {
            u64 total_path = *(cave->RiskLevels + endIndex);
            u32 cf = *(cameFrom + endIndex);
            while (cf != 0xffffffff && cf != 0)
            {
                total_path += *(cave->RiskLevels + cf);
                cf = *(cameFrom + cf);
            }
            return total_path;
        }

        u32 y = index / width;
        u32 x = index % width;

        for (u32 directionIndex = 0; directionIndex < 4; directionIndex++)
        {
            s32 newX = x + _horizontalDirections[directionIndex];
            s32 newY = y + _verticalDirections[directionIndex];

            if (newX < 0 || newX >= width || newY < 0 || newY >= height)
                continue;
            
            u32 newIndex = newY * width + newX;

            u32 tentativeScore = *(gScore + index) /* currentScore */ + *(cave->RiskLevels + newIndex) /* weight */;

            if (tentativeScore < *(gScore + newIndex))
            {
                *(cameFrom + newIndex) = index;
                *(gScore + newIndex) = tentativeScore;
                *(fScore + newIndex) = tentativeScore + (abs((s32)endY - newY) + abs((s32)endX - newX));
                
                b8 found = false;
                for (u32 openIndex = 0; openIndex < *openCount; openIndex++)
                {
                    if (*(open + openIndex) == newIndex)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    *(open + (*openCount)++) = newIndex;
            }
        }
    }

    return 0;
}

internal u64
FindPath(u32 startY, u32 startX, u32 endY, u32 endX, parse_result *cave)
{
    u32 *open = (u32 *)calloc(cave->Size.Width * cave->Size.Height, sizeof(u32));
    u32 openCount = 0;

    *(open + openCount++) = startY * cave->Size.Width + startX;

    u64 result = Travel(open, &openCount, endY, endX, cave);

    return result;
}

internal u64
Part1()
{
    file_data file = ReadToEndOfFile("input\\day15-input.txt");
    parse_result cave = Parse(file);
    u64 totalRisk = FindPath(0, 0, cave.Size.Height - 1, cave.Size.Width - 1, &cave);
    return totalRisk;
}

internal void
Magnify(parse_result *cave)
{
    u32 width = cave->Size.Width * 5;
    u32 height = cave->Size.Height * 5;

    u8 *map = (u8 *)calloc(width * height, sizeof(u8));

    u8 magnificationMap[25] = { 0, 1, 2, 3, 4,
                                1, 2, 3, 4, 5,
                                2, 3, 4, 5, 6,
                                3, 4, 5, 6, 7,
                                4, 5, 6, 7, 8 };

    for (u32 mY = 0; mY < 5; mY++)
        for (u32 mX = 0; mX < 5; mX++)
        {
            for (u32 y = 0; y < cave->Size.Height; y++)
                for (u32 x = 0; x < cave->Size.Width; x++)
                {
                    u8 magnification = *(magnificationMap + mY * 5 + mX);
                    u8 riskLevel = *(cave->RiskLevels + y * cave->Size.Width + x) + magnification;
                    if (riskLevel > 9) riskLevel -= 9;
                    
                    u32 magnifiedIndex = mY * cave->Size.Height * width + mX * cave->Size.Width;
                    *(map + magnifiedIndex + y * width + x) = riskLevel;
                }
        }
    
    free(cave->RiskLevels);
    cave->RiskLevels = map;
    cave->Size.Height = height;
    cave->Size.Width = width;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day15-input.txt");
    parse_result cave = Parse(file);
    Magnify(&cave);
    u64 totalRisk = FindPath(0, 0, cave.Size.Height - 1, cave.Size.Width - 1, &cave);
    return totalRisk;
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

    DebugLog("- Day 15 -\n");
    DebugLog("Result Part 1: %lld (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}