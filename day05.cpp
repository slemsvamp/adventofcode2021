#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct key_value_pair
{
    u32 Key;
    u32 Value;
};

struct dictionary
{
    key_value_pair *KeyValuePairs;
    u32 Count;
};

struct line
{
    u16 FromX;
    u16 FromY;
    u16 ToX;
    u16 ToY;
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

struct get_vents_result
{
    line *Vents;
    u32 Count;
};

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
Part1()
{
    file_data file = ReadToEndOfFile("input\\day05-input1.txt");
    u32 count = CountVents(file);
    get_vents_result getVentsResult = GetVents(file, count, true);

    dictionary dict = {};
    dict.KeyValuePairs = (key_value_pair *)malloc(sizeof(key_value_pair) * 1000000);
    dict.Count = 0;

    for (u32 ventIndex = 0; ventIndex < getVentsResult.Count; ventIndex++)
    {
        line vent = *(getVentsResult.Vents + ventIndex);
        
        if (vent.FromY > vent.ToY)
            vent = SwapVentY(vent);

        for (u16 ventY = vent.FromY; ventY <= vent.ToY; ventY++)
        {
            if (vent.FromX > vent.ToX)
                vent = SwapVentX(vent);

            for (u16 ventX = vent.FromX; ventX <= vent.ToX; ventX++)
            {
                u32 ventId = ventY * 1000 + ventX;
                b8 found = false;

                for (u32 kvpIndex = 0; kvpIndex < dict.Count; kvpIndex++)
                {
                    key_value_pair kvp = *(dict.KeyValuePairs + kvpIndex);

                    if (kvp.Key == ventId)
                    {
                        found = true;
                        (dict.KeyValuePairs + kvpIndex)->Value++;
                        break;
                    }
                }

                if (!found)
                {
                    *(dict.KeyValuePairs + dict.Count) = { ventId, 1 };
                    dict.Count++;
                }
            }
        }
    }

    u32 sum = 0;

    for (u32 kvpIndex = 0; kvpIndex < dict.Count; kvpIndex++)
        if ((dict.KeyValuePairs + kvpIndex)->Value >= 2)
            sum++;

    DebugLog("Result Part 1: %d\n", sum);
}

internal void
AddToDictionary(u32 ventY, u32 ventX, dictionary *dict)
{
    u32 ventId = ventY * 1000 + ventX;
    b8 found = false;

    for (u32 kvpIndex = 0; kvpIndex < dict->Count; kvpIndex++)
    {
        key_value_pair kvp = *(dict->KeyValuePairs + kvpIndex);
        if (kvp.Key == ventId)
        {
            found = true;
            (dict->KeyValuePairs + kvpIndex)->Value++;
            break;
        }
    }

    if (!found)
    {
        *(dict->KeyValuePairs + dict->Count) = {ventId, 1};
        dict->Count++;
    }
}

internal void
Part2()
{
    file_data file = ReadToEndOfFile("input\\day05-input1.txt");
    u32 count = CountVents(file);
    get_vents_result getVentsResult = GetVents(file, count, false);

    dictionary dict = {};
    dict.KeyValuePairs = (key_value_pair *)malloc(sizeof(key_value_pair) * 1000000);
    dict.Count = 0;

    for (u32 ventIndex = 0; ventIndex < getVentsResult.Count; ventIndex++)
    {
        line vent = *(getVentsResult.Vents + ventIndex);

        s16 diffY = vent.ToY - vent.FromY;
        s16 diffX = vent.ToX - vent.FromX;

        s16 stepY = diffY < 0 ? -1 : diffY > 0 ? 1 : 0;
        s16 stepX = diffX < 0 ? -1 : diffX > 0 ? 1 : 0;

        if (stepY == 0)
            for (u16 ventX = vent.FromX; ventX != vent.ToX; ventX += stepX)
                AddToDictionary(vent.FromY, ventX, &dict);
        else if (stepX == 0)
            for (u16 ventY = vent.FromY; ventY != vent.ToY; ventY += stepY)
                AddToDictionary(ventY, vent.FromX, &dict);
        else
            for (u16 ventY = vent.FromY, ventX = vent.FromX; ventY != vent.ToY && ventX != vent.ToX; ventY += stepY, ventX += stepX)
                AddToDictionary(ventY, ventX, &dict);
        AddToDictionary(vent.ToY, vent.ToX, &dict);
    }

    u32 sum = 0;
    for (u32 kvpIndex = 0; kvpIndex < dict.Count; kvpIndex++)
    {
        if ((dict.KeyValuePairs + kvpIndex)->Value >= 2)
            sum++;
    }

    DebugLog("Result Part 2: %d\n", sum);
}

u32 main(s32 argumentCount, char *arguments[])
{
    Part1();
    Part2();

    return 0;
}