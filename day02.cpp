#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

void Part1()
{
    file_data data = ReadToEndOfFile("input\\day02-input1.txt");

    s32 depth = 0;
    s32 horizontal = 0;

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        if (strstr(lines, "forward"))
            horizontal += atoi(lines + 8);
        else if (strstr(lines, "up"))
            depth -= atoi(lines + 3);
        else
            depth += atoi(lines + 5);

        lines = strtok(NULL, "\r\n");
    }

    s32 result = depth * horizontal;

    DebugLog("Result Part 1: %d\n", result);
}

void Part2()
{
    file_data data = ReadToEndOfFile("input\\day02-input1.txt");

    s32 depth = 0;
    s32 horizontal = 0;
    s32 aim = 0;

    char *lines = strtok(data.Data, "\r\n");
    while (lines != NULL)
    {
        if (strstr(lines, "forward"))
        {
            s32 move = atoi(lines + 8);
            horizontal += move;
            depth += aim * move;
        }
        else if (strstr(lines, "up"))
            aim -= atoi(lines + 3);
        else
            aim += atoi(lines + 5);

        lines = strtok(NULL, "\r\n");
    }

    s32 result = depth * horizontal;

    DebugLog("Result Part 2: %d\n", result);
}

int32_t main(s32 argumentCount, char *arguments[])
{
    Part1();
    Part2();

    return 0;
}