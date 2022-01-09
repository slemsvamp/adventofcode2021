#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

enum instruction_type
{
    instruction_type_off,
    instruction_type_on
};

struct coordinate
{
    s32 X;
    s32 Y;
    s32 Z;
};

struct cube
{
    b8 On;
    coordinate LowerBounds;
    coordinate UpperBounds;
};

struct split_result
{
    cube *Cubes;
    u32 CubeCount;
};

struct instruction
{
    instruction_type Type;
    coordinate LowerBounds;
    coordinate UpperBounds;
};

struct parse_result
{
    instruction *Instructions;
    u32 InstructionCount;
};

internal void
SkipPast(char **buffer, char character)
{
    char *playhead = *buffer;
    while (*playhead != character)
        playhead++;
    *buffer = playhead + 1;
}

internal b8
Intersects(cube left, cube right)
{
    return !(left.UpperBounds.X < right.LowerBounds.X || left.UpperBounds.Y < right.LowerBounds.Y || left.UpperBounds.Z < right.LowerBounds.Z ||
             right.UpperBounds.X < left.LowerBounds.X || right.UpperBounds.Y < left.LowerBounds.Y || right.UpperBounds.Z < left.LowerBounds.Z);
}

internal cube
Diff(cube left, cube right)
{
    cube result = {};

    return result;
}

internal parse_result
Parse(file_data file)
{
    parse_result result = {};
    instruction *instructionBuffer = (instruction *)malloc(1000 * sizeof(instruction));
    u32 instructionCount = 0;

    char *readBuffer = (char *)malloc(file.Size);
    memcpy(readBuffer, file.Data, file.Size);

    char *line = strtok(readBuffer, NEW_LINE);
    char *onOrOff = (char *)calloc(4, 1);

    s32 x1, x2, y1, y2, z1, z2;
    b8 skip = false;

    while (line)
    {
        skip = false;
        instruction i = {};
        strncpy_s(onOrOff, 4, line, 3);
        if (strcmp(onOrOff, "on ") == 0)
            i.Type = instruction_type_on;
        else if (strcmp(onOrOff, "off") == 0)
            i.Type = instruction_type_off;
        else
            skip = true;

        if (!skip)
        {
            char *searchAhead = line;

            SkipPast(&searchAhead, '=');
            x1 = (s32)atoi(searchAhead);
            SkipPast(&searchAhead, '.');
            SkipPast(&searchAhead, '.');
            x2 = (s32)atoi(searchAhead);

            SkipPast(&searchAhead, '=');
            y1 = (s32)atoi(searchAhead);
            SkipPast(&searchAhead, '.');
            SkipPast(&searchAhead, '.');
            y2 = (s32)atoi(searchAhead);

            SkipPast(&searchAhead, '=');
            z1 = (s32)atoi(searchAhead);
            SkipPast(&searchAhead, '.');
            SkipPast(&searchAhead, '.');
            z2 = (s32)atoi(searchAhead);

            if (x1 > x2)
                Swap(&x1, &x2);
            if (y1 > y2)
                Swap(&y1, &y2);
            if (z1 > z2)
                Swap(&z1, &z2);

            i.LowerBounds.X = x1;
            i.LowerBounds.Y = y1;
            i.LowerBounds.Z = z1;

            i.UpperBounds.X = x2;
            i.UpperBounds.Y = y2;
            i.UpperBounds.Z = z2;

            *(instructionBuffer + instructionCount++) = i;
        }

        line = strtok(NULL, NEW_LINE);
    }

    result.InstructionCount = instructionCount;
    result.Instructions = (instruction *)malloc(instructionCount * sizeof(instruction));
    memcpy(result.Instructions, instructionBuffer, instructionCount * sizeof(instruction));

    free(instructionBuffer);
    return result;
}

struct split_data
{
    s32 From;
    s32 To;
};

struct add_split_result
{
    split_data *Splits;
    u32 SplitCount;
};

internal add_split_result
AddSplits(s32 splitterLowerBound, s32 splitterUpperBound, s32 targetLowerBound, s32 targetUpperBound)
{
    add_split_result result = {};

    split_data *splitBuffer = (split_data *)malloc(10 * sizeof(split_data));
    u32 splitCount = 0;

    if (splitterLowerBound <= targetLowerBound)
    {
        // |.[xxx].|
        // |.[xx|xx]

        split_data sd = {};
        sd.From = targetLowerBound;

        if (splitterUpperBound >= targetUpperBound)
            sd.To = targetUpperBound;
        else
        {
            sd.To = splitterUpperBound;
            *(splitBuffer + splitCount++) = sd;

            sd.From = splitterUpperBound + 1;
            sd.To = targetUpperBound;
        }
        *(splitBuffer + splitCount++) = sd;
    }
    else if (splitterLowerBound > targetLowerBound)
    {
        // [xx|xx].|
        // [xx|x|xx]
        split_data sd = {};

        sd.From = targetLowerBound;
        sd.To = splitterLowerBound - 1;
        *(splitBuffer + splitCount++) = sd;

        sd.From = splitterLowerBound;
        
        if (splitterUpperBound < targetUpperBound)
        {
            sd.To = splitterUpperBound;
            *(splitBuffer + splitCount++) = sd;

            sd.From = splitterUpperBound + 1;
            sd.To = targetUpperBound;
        }
        else
            sd.To = targetUpperBound;

        *(splitBuffer + splitCount++) = sd;
    }

    result.SplitCount = splitCount;
    result.Splits = (split_data *)malloc(splitCount * sizeof(split_data));
    memcpy(result.Splits, splitBuffer, splitCount * sizeof(split_data));

    free(splitBuffer);

    return result;
}

internal split_result
SplitCubes(cube splitter, cube target)
{
    split_result result = {};

    add_split_result xSplitResult = AddSplits(splitter.LowerBounds.X, splitter.UpperBounds.X, target.LowerBounds.X, target.UpperBounds.X);
    add_split_result ySplitResult = AddSplits(splitter.LowerBounds.Y, splitter.UpperBounds.Y, target.LowerBounds.Y, target.UpperBounds.Y);
    add_split_result zSplitResult = AddSplits(splitter.LowerBounds.Z, splitter.UpperBounds.Z, target.LowerBounds.Z, target.UpperBounds.Z);

    cube *cubeBuffer = (cube *)malloc(1000 * sizeof(cube));
    u32 cubeCount = 0;

    if (splitter.LowerBounds.X == target.LowerBounds.X && splitter.LowerBounds.Y == target.LowerBounds.Y && splitter.LowerBounds.Z == target.LowerBounds.Z &&
        splitter.UpperBounds.X == target.UpperBounds.X && splitter.UpperBounds.Y == target.UpperBounds.Y && splitter.UpperBounds.Z == target.UpperBounds.Z)
    {
        result.CubeCount = 0;
        result.Cubes = 0;
        return result;
    }

    for (u8 xSplitIndex = 0; xSplitIndex < xSplitResult.SplitCount; xSplitIndex++)
    {
        s32 xLower = xSplitResult.Splits[xSplitIndex].From;
        s32 xUpper = xSplitResult.Splits[xSplitIndex].To;

        for (u8 ySplitIndex = 0; ySplitIndex < ySplitResult.SplitCount; ySplitIndex++)
        {
            s32 yLower = ySplitResult.Splits[ySplitIndex].From;
            s32 yUpper = ySplitResult.Splits[ySplitIndex].To;

            for (u8 zSplitIndex = 0; zSplitIndex < zSplitResult.SplitCount; zSplitIndex++)
            {
                s32 zLower = zSplitResult.Splits[zSplitIndex].From;
                s32 zUpper = zSplitResult.Splits[zSplitIndex].To;

                cube c = {};
                c.LowerBounds.X = xLower;
                c.LowerBounds.Y = yLower;
                c.LowerBounds.Z = zLower;
                c.UpperBounds.X = xUpper;
                c.UpperBounds.Y = yUpper;
                c.UpperBounds.Z = zUpper;
                c.On = target.On;

                Assert(c.UpperBounds.X >= c.LowerBounds.X);
                Assert(c.UpperBounds.Y >= c.LowerBounds.Y);
                Assert(c.UpperBounds.Z >= c.LowerBounds.Z);

                if (!Intersects(c, splitter))
                    *(cubeBuffer + cubeCount++) = c;
            }
        }
    }

    result.CubeCount = cubeCount;
    result.Cubes = (cube *)malloc(cubeCount * sizeof(cube));
    memcpy(result.Cubes, cubeBuffer, cubeCount * sizeof(cube));

    free(cubeBuffer);
    free(xSplitResult.Splits);
    free(ySplitResult.Splits);
    free(zSplitResult.Splits);
    return result;
}

internal u64
CountCubes(cube *cubes, u32 cubeCount)
{
    u64 result = 0;
    for (u32 cubeIndex = 0; cubeIndex < cubeCount; cubeIndex++)
    {
        cube c = *(cubes + cubeIndex);
        if (c.On)
            result += (u64)(c.UpperBounds.X - c.LowerBounds.X + 1) * (u64)(c.UpperBounds.Y - c.LowerBounds.Y + 1) * (u64)(c.UpperBounds.Z - c.LowerBounds.Z + 1);
    }
    return result;
}

internal void
Confine(cube *c, s32 minValue, s32 maxValue)
{
    c->LowerBounds.X = __min(maxValue, __max(minValue, c->LowerBounds.X));
    c->LowerBounds.Y = __min(maxValue, __max(minValue, c->LowerBounds.Y));
    c->LowerBounds.Z = __min(maxValue, __max(minValue, c->LowerBounds.Z));
    c->UpperBounds.X = __min(maxValue, __max(minValue, c->UpperBounds.X));
    c->UpperBounds.Y = __min(maxValue, __max(minValue, c->UpperBounds.Y));
    c->UpperBounds.Z = __min(maxValue, __max(minValue, c->UpperBounds.Z));
}

internal u64
Part1()
{
    file_data file = ReadToEndOfFile("input\\day22-input.txt");
    parse_result parseResult = Parse(file);

    cube *cubes = (cube *)malloc(1000 * sizeof(cube));
    u32 cubeCount = 0;

    cube *cubeBuffer = (cube *)malloc(1000 * sizeof(cube));
    u32 cubeBufferCount = 0;

    cube bounds = {};
    bounds.LowerBounds.X = -50;
    bounds.UpperBounds.X = 50;
    bounds.LowerBounds.Y = -50;
    bounds.UpperBounds.Y = 50;
    bounds.LowerBounds.Z = -50;
    bounds.UpperBounds.Z = 50;

    u32 *indicesToRemove = (u32 *)malloc(1000 * sizeof(u32));
    u32 indicesCount = 0;

    for (u32 instructionIndex = 0; instructionIndex < parseResult.InstructionCount; instructionIndex++)
    {
        instruction inst = *(parseResult.Instructions + instructionIndex);

        cube splitterCube = {};

        splitterCube.LowerBounds = inst.LowerBounds;
        splitterCube.UpperBounds = inst.UpperBounds;
        splitterCube.On = inst.Type == instruction_type_on;

        if (Intersects(splitterCube, bounds))
        {
            Confine(&splitterCube, -50, 50);
            *(cubes + cubeCount++) = splitterCube;

            u32 endIndex = cubeCount - 1;

            indicesCount = 0;

            for (u32 cubeIndex = 0; cubeIndex < endIndex; cubeIndex++)
            {
                cube targetCube = *(cubes + cubeIndex);

                b8 shouldSplit = Intersects(splitterCube, targetCube);

                if (shouldSplit)
                {
                    split_result splitResult = SplitCubes(splitterCube, targetCube);

                    if (splitResult.CubeCount > 0)
                        for (u32 splitsIndex = 0; splitsIndex < splitResult.CubeCount; splitsIndex++)
                            *(cubes + cubeCount++) = *(splitResult.Cubes + splitsIndex);

                    *(indicesToRemove + indicesCount++) = cubeIndex;
                }
            }

            cubeBufferCount = 0;
            for (u32 cubeIndex = 0; cubeIndex < cubeCount; cubeIndex++)
            {
                b8 add = true;
                for (u32 removeIndex = 0; removeIndex < indicesCount; removeIndex++)
                    if (*(indicesToRemove + removeIndex) == cubeIndex)
                    {
                        add = false;
                        break;
                    }

                if (add)
                    *(cubeBuffer + cubeBufferCount++) = *(cubes + cubeIndex);
            }
        }

        cubeCount = cubeBufferCount;
        memcpy(cubes, cubeBuffer, cubeBufferCount * sizeof(cube));
    }

    free(cubeBuffer);
    free(indicesToRemove);

    u64 result = CountCubes(cubes, cubeCount);

    return result;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day22-input.txt");
    parse_result parseResult = Parse(file);

    cube *cubes = (cube *)malloc(100000 * sizeof(cube));
    u32 cubeCount = 0;

    cube *cubeBuffer = (cube *)malloc(100000 * sizeof(cube));
    u32 cubeBufferCount = 0;

    u32 *indicesToRemove = (u32 *)malloc(1000 * sizeof(u32));
    u32 indicesCount = 0;

    for (u32 instructionIndex = 0; instructionIndex < parseResult.InstructionCount; instructionIndex++)
    {
        instruction inst = *(parseResult.Instructions + instructionIndex);

        cube splitterCube = {};

        splitterCube.LowerBounds = inst.LowerBounds;
        splitterCube.UpperBounds = inst.UpperBounds;
        splitterCube.On = inst.Type == instruction_type_on;

        *(cubes + cubeCount++) = splitterCube;

        u32 endIndex = cubeCount - 1;

        indicesCount = 0;

        for (u32 cubeIndex = 0; cubeIndex < endIndex; cubeIndex++)
        {
            cube targetCube = *(cubes + cubeIndex);

            b8 shouldSplit = Intersects(splitterCube, targetCube);

            if (shouldSplit)
            {
                split_result splitResult = SplitCubes(splitterCube, targetCube);

                if (splitResult.CubeCount > 0)
                    for (u32 splitsIndex = 0; splitsIndex < splitResult.CubeCount; splitsIndex++)
                        *(cubes + cubeCount++) = *(splitResult.Cubes + splitsIndex);

                *(indicesToRemove + indicesCount++) = cubeIndex;
            }
        }

        cubeBufferCount = 0;
        for (u32 cubeIndex = 0; cubeIndex < cubeCount; cubeIndex++)
        {
            b8 add = true;
            for (u32 removeIndex = 0; removeIndex < indicesCount; removeIndex++)
                if (*(indicesToRemove + removeIndex) == cubeIndex)
                {
                    add = false;
                    break;
                }

            if (add)
                *(cubeBuffer + cubeBufferCount++) = *(cubes + cubeIndex);
        }

        Assert(cubeBufferCount < 100000);

        cubeCount = cubeBufferCount;
        memcpy(cubes, cubeBuffer, cubeBufferCount * sizeof(cube));
    }

    free(cubeBuffer);
    free(indicesToRemove);

    u64 result = CountCubes(cubes, cubeCount);

    return result;
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

    DebugLog("- Day 22 -\n");
    DebugLog("Result Part 1: %llu (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %llu (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}