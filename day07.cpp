#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct parse_result
{
    u32 *Values;
    u32 Count;
};

internal u32
CountValues(file_data file)
{
    char *playhead = file.Data;
    u32 commas = 0;
    while ((playhead - file.Data) < file.Size)
    {
        if (*playhead == ',')
            commas++;
        playhead++;
    }
    u32 result = commas + 1;
    return result;
}

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    result.Count = CountValues(file);
    result.Values = (u32 *)malloc(sizeof(u32) * result.Count);
    
    u32 valueIndex = 0;
    char *number = strtok(file.Data, ",");

    while (number != NULL)
    {
        *(result.Values + valueIndex++) = atoi(number);
        number = strtok(NULL, ",");
    }

    return result;
}

internal void
Swap(u32 *one, u32 *other)
{
    u32 temp = *one;
    *one = *other;
    *other = temp;
}

internal u32
Partition(u32 *values, s32 low, s32 high) 
{ 
    u32 pivot = *(values + high);
    s32 lowerIndex = (low - 1);

    for (u32 index = low; index <= high - 1; index++)
    {
        if (*(values + index) < pivot)
        {
            lowerIndex++;
            Swap(values + lowerIndex, values + index);
        }
    }
    Swap(values + lowerIndex + 1, values + high);
    return lowerIndex + 1;
} 

internal void
QuickSort(u32 *values, s32 low, s32 high)
{
    if (low < high)
    { 
        u32 partitionIndex = Partition(values, low, high); 
        QuickSort(values, low, partitionIndex - 1); 
        QuickSort(values, partitionIndex + 1, high); 
    } 
}

internal u32
CalculateTotalCost(u32 halfWay, u32 *values, u32 count, b8 constantRate)
{
    u64 result = 0;

    u32 *playhead = values;
    while (*playhead < halfWay)
    {
        u32 distance = halfWay - *playhead;
        if (constantRate)
            result += distance;
        else
            result += distance * (distance + 1) / 2;
        playhead++;
    }

    while (playhead - values < count)
    {
        u32 distance = *playhead - halfWay;
        if (constantRate)
            result += distance;
        else
            result += distance * (distance + 1) / 2;
        playhead++;
    }

    return result;
}

internal void
Part1()
{
    file_data file = ReadToEndOfFile("input\\day07-input1.txt");
    parse_result parseResult = Parse(file);

    QuickSort(parseResult.Values, 0, parseResult.Count - 1);

    u32 lastCost = CalculateTotalCost(0, parseResult.Values, parseResult.Count, true);

    for (u32 index = 0; index < parseResult.Count - 1; index++)
    {
        u32 cost = CalculateTotalCost(index, parseResult.Values, parseResult.Count, true);

        if (cost > lastCost)
        {
            DebugLog("Part 1 Cost: %d\n", lastCost);
            return;
        }
        
        lastCost = cost;
    }
}

internal void
Part2()
{
    file_data file = ReadToEndOfFile("input\\day07-input1.txt");
    parse_result parseResult = Parse(file);

    QuickSort(parseResult.Values, 0, parseResult.Count - 1);

    u32 lastCost = CalculateTotalCost(0, parseResult.Values, parseResult.Count, false);

    for (u32 index = 0; index < parseResult.Count - 1; index++)
    {
        u32 cost = CalculateTotalCost(index, parseResult.Values, parseResult.Count, false);

        if (cost > lastCost)
        {
            DebugLog("Part 2 Cost: %d\n", lastCost);
            return;
        }
        
        lastCost = cost;
    }
}

u32
main(s32 argumentCount, char *arguments[])
{
    Part1();
    Part2();

    return 0;
}