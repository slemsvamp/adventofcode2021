#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct line
{
    u16 FromX;
    u16 FromY;
    u16 ToX;
    u16 ToY;
};

struct node
{
    u32 Hash;
    node *Next;
};

struct get_vents_result
{
    line *Vents;
    u32 Count;
};

internal u32
CountVents(file_data file)
{
    char *playhead = file.Data;
    u32 lines = 0;
    while (playhead != NULL && (playhead - file.Data + 1) < (u32)file.Size)
    {
        if (*playhead == '\r' && *(playhead + 1) == '\n')
            lines++;
        playhead++;
    }
    u32 result = lines + 1;
    return result;
}

internal line
GetVent(char **playheadAddress)
{
    line result = {};

    char *playhead = *playheadAddress;

    result.FromX = (u16)atoi(playhead);
    while (*playhead != ',')
        playhead++;
    playhead++;
    result.FromY = (u16)atoi(playhead);
    while (*playhead != '>')
        playhead++;
    while (*playhead < '0' || *playhead > '9')
        playhead++;
    result.ToX = (u16)atoi(playhead);
    while (*playhead != ',')
        playhead++;
    playhead++;
    result.ToY = (u16)atoi(playhead);
    while (*playhead != '\r')
        playhead++;
    playhead += 2;

    *playheadAddress = playhead;

    return result;
}

internal get_vents_result
GetVents(file_data file, u32 count, b8 orthogonalOnly)
{
    char *playhead = file.Data;

    line *buffer = (line *)malloc(sizeof(line) * count);
    u32 ventIndex = 0;
    u32 actualCount = 0;

    while (playhead != NULL && (playhead - file.Data) < (u32)file.Size)
    {
        line vent = GetVent(&playhead);

        if ((orthogonalOnly && (vent.FromX == vent.ToX || vent.FromY == vent.ToY)) || !orthogonalOnly)
        {
            *(buffer + ventIndex) = vent;
            actualCount++;
            ventIndex++;
        }
    }

    get_vents_result result = {};
    result.Count = count;

    if (actualCount == count)
        result.Vents = buffer;
    else
    {
        result.Vents = (line *)malloc(sizeof(line) * actualCount);
        memcpy(result.Vents, buffer, sizeof(line) * actualCount);
        free(buffer);
        result.Count = actualCount;
    }

    return result;
}

internal line
SwapVentY(line vent)
{
    u16 temp = vent.FromY;
    vent.FromY = vent.ToY;
    vent.ToY = temp;
    return vent;
}

internal line
SwapVentX(line vent)
{
    u16 temp = vent.FromX;
    vent.FromX = vent.ToX;
    vent.ToX = temp;
    return vent;
}

internal void
Add(u16 y, u16 x, u32 *dict, node **hashes)
{
    u32 hash = y * 1000 + x;
    u32 *value = dict + hash;

    if (!*value)
        *value = 1;
    else
    {
        if (!*hashes)
        {
            node *addNode = (node *)malloc(sizeof(node));
            addNode->Hash = hash;
            addNode->Next = NULL;
            *hashes = addNode;
        }
        else
        {
            node *hashed = *hashes;
            while (hashed && hashed->Next)
            {
                if (hashed->Hash == hash)
                    return;
                hashed = hashed->Next;
            }
            node *addNode = (node *)malloc(sizeof(node));
            addNode->Hash = hash;
            addNode->Next = NULL;
            hashed->Next = addNode;
        }
    }
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day05-input.txt");
    u32 count = CountVents(file);
    get_vents_result getVentsResult = GetVents(file, count, true);

    u32 *dict = (u32 *)calloc(1000000, sizeof(u32));
    node *nodes = NULL;

    for (u32 ventIndex = 0; ventIndex < getVentsResult.Count; ventIndex++)
    {
        line vent = *(getVentsResult.Vents + ventIndex);
        
        if (vent.FromY > vent.ToY)
            vent = SwapVentY(vent);

        if (vent.FromX > vent.ToX)
            vent = SwapVentX(vent);

        for (u16 ventY = vent.FromY; ventY <= vent.ToY; ventY++)
            for (u16 ventX = vent.FromX; ventX <= vent.ToX; ventX++)
                Add(ventY, ventX, dict, &nodes);
    }

    u32 sum = 0;

    while (nodes)
    {
        sum++;
        nodes = nodes->Next;
    }

    return sum;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day05-input.txt");
    u32 count = CountVents(file);
    get_vents_result getVentsResult = GetVents(file, count, false);

    u32 *dict = (u32 *)calloc(1000000, sizeof(u32));
    node *nodes = NULL;

    for (u32 ventIndex = 0; ventIndex < getVentsResult.Count; ventIndex++)
    {
        line vent = *(getVentsResult.Vents + ventIndex);

        s16 diffY = vent.ToY - vent.FromY;
        s16 diffX = vent.ToX - vent.FromX;

        s16 stepY = diffY < 0 ? -1 : diffY > 0 ? 1 : 0;
        s16 stepX = diffX < 0 ? -1 : diffX > 0 ? 1 : 0;

        if (stepY == 0)
            for (u16 ventX = vent.FromX; ventX != vent.ToX; ventX += stepX)
                Add(vent.FromY, ventX, dict, &nodes);
        else if (stepX == 0)
            for (u16 ventY = vent.FromY; ventY != vent.ToY; ventY += stepY)
                Add(ventY, vent.FromX, dict, &nodes);
        else
            for (u16 ventY = vent.FromY, ventX = vent.FromX; ventY != vent.ToY && ventX != vent.ToX; ventY += stepY, ventX += stepX)
                Add(ventY, ventX, dict, &nodes);
        Add(vent.ToY, vent.ToX, dict, &nodes);
    }

    u32 sum = 0;

    while (nodes)
    {
        sum++;
        nodes = nodes->Next;
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

    u32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 05 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}