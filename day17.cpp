#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct velocity
{
    s32 X;
    s32 Y;
};

struct coordinate
{
    s32 X;
    s32 Y;
};

struct area
{
    coordinate topLeft;
    coordinate bottomRight;
};

internal velocity
Drag(velocity v)
{
    if (v.X > 0)
        return {v.X - 1, v.Y - 1};
    if (v.X < 0)
        return {v.X + 1, v.Y - 1};
    return {v.X, v.Y - 1};
}

struct simulation_data
{
    b8 OvershotX;
    b8 OvershotY;
    b8 Hit;
    s32 MaxY;
};

internal simulation_data
Simulate(velocity initialVelocity, area targetArea)
{
    coordinate c = {0,0};
    velocity v = initialVelocity;
    s32 maxY = 0;

    for (;;)
    {
        c.X += v.X;
        c.Y += v.Y;
        v = Drag(v);

        if (c.Y > maxY)
            maxY = c.Y;

        if (c.X > targetArea.bottomRight.X)
            return { c.X > targetArea.bottomRight.X, c.Y < targetArea.bottomRight.Y, false, maxY };
        else if (c.Y < targetArea.bottomRight.Y)
            return { c.X > targetArea.bottomRight.X, c.Y < targetArea.bottomRight.Y, false, maxY };
        else if (c.X >= targetArea.topLeft.X && c.X <= targetArea.bottomRight.X && c.Y <= targetArea.topLeft.Y && c.Y >= targetArea.bottomRight.Y)
            return { c.X > targetArea.bottomRight.X, c.Y < targetArea.bottomRight.Y, true, maxY };
    }
}

internal area
Parse(file_data file)
{
    area result = {};
    char *playhead = file.Data;
    while (*playhead < '0' || *playhead > '9') playhead++;
    result.topLeft.X = atoi(playhead);
    while (*playhead >= '0' && *playhead <= '9') playhead++;
    while (*playhead < '0' || *playhead > '9') playhead++;
    result.bottomRight.X = atoi(playhead);
    while (*playhead >= '0' && *playhead <= '9') playhead++;
    while (*playhead < '0' || *playhead > '9') playhead++;
    result.bottomRight.Y = 0 - atoi(playhead);
    while (*playhead >= '0' && *playhead <= '9') playhead++;
    while (*playhead < '0' || *playhead > '9') playhead++;
    result.topLeft.Y = 0 - atoi(playhead);
    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day17-input.txt");
    area targetArea = Parse(file);
    u32 maxY = 0;

    for (s32 testX = 1; testX < targetArea.bottomRight.X + 1; testX++)
        for (s32 testY = targetArea.bottomRight.Y; testY < -targetArea.bottomRight.Y; testY++)
        {
            velocity initialVelocity = { testX, testY };
            simulation_data data = Simulate(initialVelocity, targetArea);
            
            if (data.Hit && data.MaxY > maxY)
                maxY = data.MaxY;
            if (data.OvershotX)
                break;
        }

    return maxY;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day17-input.txt");
    area targetArea = Parse(file);
    u32 hits = 0;

    for (s32 testX = 1; testX < targetArea.bottomRight.X + 1; testX++)
        for (s32 testY = targetArea.bottomRight.Y; testY < -targetArea.bottomRight.Y; testY++)
        {
            velocity initialVelocity = { testX, testY };
            simulation_data data = Simulate(initialVelocity, targetArea);
            
            if (data.Hit)
                hits++;
            if (data.OvershotX)
                break;
        }

    return hits;
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

    DebugLog("- Day 17 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}