#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char NEW_LINE[4] = "\r\n";

struct node
{
    u32 Index;
    char Name[3];
    b8 Small;
};

struct edge
{
    u32 SmallerIndex;
    u32 LargerIndex;
};

struct graph
{
    u32 StartIndex;
    u32 EndIndex;

    node Nodes[20];
    edge Edges[22];

    u32 NodeCount;
    u32 EdgeCount;
};

internal node
ParseNode(char *line)
{
    node result = {};
    memcpy(result.Name, line, 2);
    result.Small = result.Name[0] >= 'a' && result.Name[0] <= 'z';
    return result;
}

internal void
GraphInsertEdge(graph *cave, node *n1, node *n2)
{
    if (n2->Index < n1->Index)
    {
        node *temp = n1;
        n1 = n2;
        n2 = temp;
    }

    b8 found = false;
    for (u32 edgeIndex = 0; edgeIndex < cave->EdgeCount; edgeIndex++)
    {
        if (cave->Edges[edgeIndex].SmallerIndex == n1->Index && cave->Edges[edgeIndex].LargerIndex == n2->Index)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        edge dg = {};
        dg.SmallerIndex = n1->Index;
        dg.LargerIndex = n2->Index;

        cave->Edges[cave->EdgeCount++] = dg;
    }
}

internal void
GraphInsertNode(graph *cave, node *n)
{
    b8 found = 0;
    for (u32 nodeIndex = 0; nodeIndex < cave->NodeCount; nodeIndex++)
        if (n->Name[0] == cave->Nodes[nodeIndex].Name[0] && n->Name[1] == cave->Nodes[nodeIndex].Name[1])
        {
            found = 1;
            n->Index = nodeIndex;
            break;
        }

    if (!found)
    {
        if (strstr(n->Name, "st"))
            cave->StartIndex = cave->NodeCount;
        if (strstr(n->Name, "en"))
            cave->EndIndex = cave->NodeCount;
        n->Index = cave->NodeCount;
        cave->Nodes[cave->NodeCount++] = *n;
    }
}

internal graph
Parse(file_data file)
{
    graph result = {};

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);

    while (line)
    {
        node nd[2] = {};

        char *playhead = line;

        nd[0] = ParseNode(playhead);
        GraphInsertNode(&result, &nd[0]);

        while (*playhead != '-')
            playhead++;
        playhead++;

        nd[1] = ParseNode(playhead);
        GraphInsertNode(&result, &nd[1]);

        GraphInsertEdge(&result, &nd[0], &nd[1]);

        line = strtok(NULL, NEW_LINE);
    }

    return result;
}

internal char*
GenerateTraversalText(graph *cave, u32 *visitedCaves, u32 visitedCount)
{
    char buffer[4096] = {};
    u32 count = 0;

    for (u32 traversalIndex = 0; traversalIndex < visitedCount; traversalIndex++)
    {
        node traversed = *(cave->Nodes + *(visitedCaves + traversalIndex));
        
        if (traversed.Index == cave->StartIndex)
        {
            memcpy(buffer + count, "start", 5);
            count += 5;
        }
        else if (traversed.Index == cave->EndIndex)
        {
            memcpy(buffer + count, "end", 3);
            count += 3;
        }
        else
        {
            for (u32 characterIndex = 0; characterIndex < strlen(traversed.Name); characterIndex++)
                *(buffer + count++) = *(traversed.Name + characterIndex);
        }

        if (traversalIndex != visitedCount - 1)
            *(buffer + count++) = ',';
    }

    char *result = (char *)malloc(count + 1);
    memcpy(result, buffer, count);
    *(result + count) = 0;

    return result;
}

internal u32
Traverse(graph *cave, node from, node end, u32 *visitedCaves, u32 visitedCount, b8 allowVisitingSmallCavesTwice, b8 visitedTwice)
{
    u32 sum = 0;

    *(visitedCaves + visitedCount++) = from.Index;

    if (from.Index == cave->EndIndex)
        return 1;

    Assert(visitedCount < 30);

    // NOTE: i'd prefer a faster way than iterating through all these, maybe that nodes have a list of their edges, but for development speed i sacrificed it
    for (u32 edgeIndex = 0; edgeIndex < cave->EdgeCount; edgeIndex++)
    {
        edge dg = *(cave->Edges + edgeIndex);
        node *to = 0;

        if (dg.SmallerIndex == from.Index)
            to = cave->Nodes + dg.LargerIndex;
        else if (dg.LargerIndex == from.Index)
            to = cave->Nodes + dg.SmallerIndex;
        else
            continue;

        if (to->Small)
        {
            if (to->Index == cave->StartIndex)
                continue;

            if (!allowVisitingSmallCavesTwice)
            {
                b8 visitedCave = 0;
                for (u32 visitedIndex = 0; visitedIndex < visitedCount; visitedIndex++)
                    if (*(visitedCaves + visitedIndex) == to->Index)
                    {
                        visitedCave = 1;
                        break;
                    }

                if (!visitedCave)
                {
                    u32 *newVisitedCaves = (u32 *)calloc(visitedCount + 1, sizeof(u32));
                    memcpy(newVisitedCaves, visitedCaves, sizeof(u32) * visitedCount);
                    sum += Traverse(cave, *to, end, newVisitedCaves, visitedCount, allowVisitingSmallCavesTwice, visitedTwice);
                    free(newVisitedCaves);
                }
            }
            else
            {
                u8 visitedCave = 0;
                for (u32 visitedIndex = 0; visitedIndex < visitedCount; visitedIndex++)
                    if (*(visitedCaves + visitedIndex) == to->Index)
                        visitedCave += 1;

                if (visitedCave == 0 || (visitedCave == 1 && !visitedTwice))
                {
                    b8 newVisitedTwice = visitedTwice;

                    if (visitedCave == 1)
                        newVisitedTwice = 1;

                    u32 *newVisitedCaves = (u32 *)calloc(visitedCount + 1, sizeof(u32));
                    memcpy(newVisitedCaves, visitedCaves, sizeof(u32) * visitedCount);
                    sum += Traverse(cave, *to, end, newVisitedCaves, visitedCount, allowVisitingSmallCavesTwice, newVisitedTwice);
                    free(newVisitedCaves);
                }
            }
        }
        else
        {
            u32 *newVisitedCaves = (u32 *)calloc(visitedCount + 1, sizeof(u32));
            memcpy(newVisitedCaves, visitedCaves, sizeof(u32) * visitedCount);
            sum += Traverse(cave, *to, end, newVisitedCaves, visitedCount, allowVisitingSmallCavesTwice, visitedTwice);
            free(newVisitedCaves);
        }
    }

    return sum;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day12-input1.txt");
    graph cave = Parse(file);

    node start = *(cave.Nodes + cave.StartIndex);
    node end = *(cave.Nodes + cave.EndIndex);

    u32 visitedCaves[1] = {};
    u32 visitedCount = 0;

    return Traverse(&cave, start, end, visitedCaves, visitedCount, 0, 0);
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day12-input1.txt");
    graph cave = Parse(file);

    node start = *(cave.Nodes + cave.StartIndex);
    node end = *(cave.Nodes + cave.EndIndex);

    u32 visitedCaves[1] = {};
    u32 visitedCount = 0;

    return Traverse(&cave, start, end, visitedCaves, visitedCount, 1, 0);
}

u32 main(s32 argumentCount, char *arguments[])
{
    clock_t startTime = clock();
    u64 startCycles = __rdtsc();

    u32 resultPart1 = Part1();

    clock_t part1Time = clock();
    u64 part1Cycles = __rdtsc();

    u32 resultPart2 = Part2();

    clock_t endTime = clock();
    u64 endCycles = __rdtsc();

    DebugLog("- Day 12 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}