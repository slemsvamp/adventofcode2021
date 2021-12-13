#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

struct coordinate
{
    u32 FromTop;
    u32 FromLeft;
};

enum fold_along_axis
{
    fold_along_axis_x,
    fold_along_axis_y
};

struct fold_along
{
    fold_along_axis Axis;
    u32 Position;
};

struct min_max
{
    u32 Min = 0xffffffff;
    u32 Max = 0;
};

struct parse_result
{
    coordinate *Dots;
    u32 DotCount;
    fold_along *FoldInstructions;
    u32 FoldInstructionCount;
    min_max X;
    min_max Y;
};

internal parse_result
Parse(file_data file)
{
    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);

    parse_result result = {};

    u32 dotCount = 0;
    u32 foldInstructionCount = 0;

    while (line)
    {
        if (strstr(line, "fold along "))
            foldInstructionCount++;
        else if (strcmp(line, "") != 0)
            dotCount++;

        line = strtok(NULL, NEW_LINE);
    }

    memcpy(buffer, file.Data, file.Size);

    result.Dots = (coordinate *)calloc(dotCount, sizeof(coordinate));
    result.DotCount = dotCount;
    result.FoldInstructions = (fold_along *)calloc(foldInstructionCount, sizeof(fold_along));
    result.FoldInstructionCount = foldInstructionCount;

    dotCount = 0;
    foldInstructionCount = 0;

    line = strtok(buffer, NEW_LINE);

    while (line)
    {
        if (strstr(line, "fold along "))
        {
            fold_along foldInstruction = {};

            foldInstruction.Axis = *(line + 11) == 'x' ? fold_along_axis_x : fold_along_axis_y;
            foldInstruction.Position = atoi(line + 13);

            *(result.FoldInstructions + foldInstructionCount++) = foldInstruction;
        }
        else if (strcmp(line, "") != 0)
        {
            coordinate dot = {};
            char *playhead = line;

            dot.FromLeft = atoi(playhead);

            while (*playhead != ',')
                playhead++;
            playhead++;

            dot.FromTop = atoi(playhead);

            if (dot.FromLeft > result.X.Max) result.X.Max = dot.FromLeft;
            if (dot.FromLeft < result.X.Min) result.X.Min = dot.FromLeft;
            if (dot.FromTop > result.Y.Max) result.Y.Max = dot.FromTop;
            if (dot.FromTop < result.Y.Min) result.Y.Min = dot.FromTop;

            *(result.Dots + dotCount++) = dot;
        }

        line = strtok(NULL, NEW_LINE);
    }

    return result;
}

internal void
Fold(parse_result *parseResult, u32 foldInstructionIndex)
{
    min_max minMaxX, minMaxY;
    b8 *collisionMap = (b8 *)calloc((parseResult->X.Max + 1) * (parseResult->Y.Max + 1), sizeof(b8));

    fold_along foldInstruction = *(parseResult->FoldInstructions + foldInstructionIndex);

    coordinate *dots = (coordinate *)calloc(parseResult->DotCount, sizeof(coordinate));
    u32 dotCount = 0;

    for (u32 dotIndex = 0; dotIndex < parseResult->DotCount; dotIndex++)
    {
        coordinate dot = *(parseResult->Dots + dotIndex);

        if (foldInstruction.Axis == fold_along_axis_y && dot.FromTop > foldInstruction.Position)
            dot.FromTop = 2 * foldInstruction.Position - dot.FromTop;
        if (foldInstruction.Axis == fold_along_axis_x && dot.FromLeft > foldInstruction.Position)
            dot.FromLeft = 2 * foldInstruction.Position - dot.FromLeft;

        if (dot.FromTop > minMaxY.Max) minMaxY.Max = dot.FromTop;
        if (dot.FromTop < minMaxY.Min) minMaxY.Min = dot.FromTop;
        if (dot.FromLeft > minMaxX.Max) minMaxX.Max = dot.FromLeft;
        if (dot.FromLeft < minMaxX.Min) minMaxX.Min = dot.FromLeft;

        u32 collisionHash = dot.FromTop * (parseResult->X.Max + 1) + dot.FromLeft;

        if (!*(collisionMap + collisionHash))
        {
            *(dots + dotCount++) = dot;
            *(collisionMap + collisionHash) = 1;
        }
    }

    free(parseResult->Dots);
    free(collisionMap);

    parseResult->Dots = dots;
    parseResult->DotCount = dotCount;
    parseResult->X = minMaxX;
    parseResult->Y = minMaxY;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day13-input.txt");
    parse_result parseResult = Parse(file);

    for (u32 foldInstructionIndex = 0; foldInstructionIndex < 1; foldInstructionIndex++)
        Fold(&parseResult, foldInstructionIndex);

    return parseResult.DotCount;
}

internal void
PrintParseResult(parse_result parseResult)
{
    u32 rows = parseResult.Y.Max + 1;
    u32 columns = parseResult.X.Max + 1;
    
    char *buffer = (char *)malloc(rows * columns);

    for (u32 bufferIndex = 0; bufferIndex < rows * columns; bufferIndex++)
        *(buffer + bufferIndex) = ' ';

    for (u32 dotIndex = 0; dotIndex < parseResult.DotCount; dotIndex++)
    {
        coordinate dot = *(parseResult.Dots + dotIndex);
        *(buffer + (dot.FromTop * columns + dot.FromLeft)) = '#';
    }

    for (u32 heightIndex = 0; heightIndex < rows; heightIndex++)
    {
        char *lineBuffer = (char *)malloc(columns);
        memcpy(lineBuffer, buffer + (heightIndex * columns), columns);
        DebugLog("%s\n", lineBuffer);
        free(lineBuffer);
    }
    
    free(buffer);
}

internal parse_result
Part2()
{
    file_data file = ReadToEndOfFile("input\\day13-input.txt");
    parse_result parseResult = Parse(file);

    for (u32 foldInstructionIndex = 0; foldInstructionIndex < parseResult.FoldInstructionCount; foldInstructionIndex++)
        Fold(&parseResult, foldInstructionIndex);

    return parseResult;
}

u32 main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    parse_result resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 13 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2:\n");
    PrintParseResult(resultPart2);
    DebugLog("(%d ms, %lld cycles passed)\n", (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}