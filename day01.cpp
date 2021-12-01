#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

void Part1()
{
    u32 depthCompare = 0;
    u32 increases = 0; 
    
    file_data data = ReadToEndOfFile("input\\day01-input1.txt");

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        u32 depth = atoi(lines);
        if (depthCompare && depth > depthCompare)
            increases++;
        depthCompare = depth;
        lines = strtok(NULL, "\r\n");
    }

    DebugLog("Result Part 1: %d\n", increases);
}

void Part2()
{
    u32 depths[3] = {0, 0, 0};
    u32 depthCompare = 0;
    u32 linePlayhead = 0;
    u32 increases = 0;

    file_data data = ReadToEndOfFile("input\\day01-input1.txt");

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        u32 depth = atoi(lines);
        u32 resetIndex = linePlayhead % 3;
        
        for (u32 v = 0; v <= __min(linePlayhead, 2); v++)
            depths[v] = resetIndex == v ? depth : depths[v] + depth;

        if (linePlayhead >= 2)
        {
            u32 classIndex = (linePlayhead + 1) % 3;
            if (depthCompare && depths[classIndex] > depthCompare)
                increases++;
            depthCompare = depths[classIndex];
        }
        
        linePlayhead++;
        lines = strtok(NULL, "\r\n");
    }

    DebugLog("Result Part 2: %d\n", increases);
}

int32_t main(s32 argumentCount, char *arguments[])
{
    Part1();
    Part2();

    return 0;
}