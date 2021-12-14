#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

struct insertion_rule
{
    char Pair[2];
    char Insert;
};

struct pair_occurrence
{
    char Pair[2];
    u64 Quantity;
};

struct parse_result
{
    char LastElement;
    pair_occurrence *Occurrences;
    insertion_rule *Rules;
    u32 RuleCount;
};

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    char *buffer = (char *)malloc(file.Size);
    memcpy(buffer, file.Data, file.Size);

    char *line = strtok(buffer, NEW_LINE);
    char *polymer = line;

    line = strtok(NULL, NEW_LINE);

    pair_occurrence *occurrenceBuffer = (pair_occurrence *)malloc(sizeof(pair_occurrence) * 100);
    insertion_rule *ruleBuffer = (insertion_rule *)malloc(sizeof(insertion_rule) * 100);
    u32 ruleBufferCount = 0;

    while (line)
    {
        Assert(ruleBufferCount < 100);

        insertion_rule *rule = (ruleBuffer + ruleBufferCount);
        rule->Pair[0] = *(line);
        rule->Pair[1] = *(line+1);
        rule->Insert = *(line+6);

        pair_occurrence *occurrence = (occurrenceBuffer + ruleBufferCount++);
        occurrence->Pair[0] = rule->Pair[0];
        occurrence->Pair[1] = rule->Pair[1];
        occurrence->Quantity = 0;

        line = strtok(NULL, NEW_LINE);
    }
    
    result.RuleCount = ruleBufferCount;

    result.Rules = (insertion_rule *)malloc(sizeof(insertion_rule) * result.RuleCount);
    memcpy(result.Rules, ruleBuffer, sizeof(insertion_rule) * result.RuleCount);

    result.Occurrences = (pair_occurrence *)malloc(sizeof(pair_occurrence) * result.RuleCount);
    memcpy(result.Occurrences, occurrenceBuffer, sizeof(pair_occurrence) * result.RuleCount);

    u32 polymerTemplateLength = strlen(polymer);

    for (u32 elementIndex = 0; elementIndex < polymerTemplateLength - 1; elementIndex++)
    {
        b8 found = false;
        for (u32 pairIndex = 0; pairIndex < result.RuleCount; pairIndex++)
        {
            pair_occurrence *occurrence = (result.Occurrences + pairIndex);
            if (occurrence->Pair[0] == *(polymer + elementIndex) && occurrence->Pair[1] == *(polymer + elementIndex + 1))
                occurrence->Quantity++;
        }
    }

    result.LastElement = *(polymer + polymerTemplateLength - 1);

    return result;
}

internal u64
Analyze(pair_occurrence *pairs, u32 ruleCount, char lastElement)
{
    u32 elementsSize = 'Z' - 'A' + 1;
    u64 minQuantity = 0xffffffffffffffff;
    u64 maxQuantity = 0;
    u64 *elements = (u64 *)calloc(elementsSize, sizeof(u64));

    for (u32 occurrenceIndex = 0; occurrenceIndex < ruleCount; occurrenceIndex++)
    {
        pair_occurrence occurrence = *(pairs + occurrenceIndex);
        *(elements + occurrence.Pair[0] - 'A') += occurrence.Quantity;
    }

    (*(elements + lastElement - 'A'))++;

    for (u32 index = 0; index < elementsSize; index++)
    {
        u64 value = *(elements + index);
        if (value == 0) continue;
        if (value > maxQuantity) maxQuantity = value;
        if (value < minQuantity) minQuantity = value;
    }

    // I'll be honest, this isn't needed, if you have more than 1 element present, but..
    if (minQuantity == 0xffffffffffffffff) minQuantity = 0;

    return maxQuantity - minQuantity;
}

internal pair_occurrence*
Sequence(pair_occurrence *pairs, insertion_rule *rules, u32 ruleCount)
{
    pair_occurrence *buffer = (pair_occurrence *)malloc(sizeof(pair_occurrence) * ruleCount);
    memcpy(buffer, pairs, sizeof(pair_occurrence) * ruleCount);

    for (u32 pairIndex = 0; pairIndex < ruleCount; pairIndex++)
    {
        pair_occurrence current = *(pairs + pairIndex);

        if (current.Quantity > 0)
        {
            for (u32 ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++)
            {
                insertion_rule rule = *(rules + ruleIndex);

                if (current.Pair[0] == rule.Pair[0] && current.Pair[1] == rule.Pair[1])
                {
                    pair_occurrence createdA = {};
                    createdA.Pair[0] = current.Pair[0];
                    createdA.Pair[1] = rule.Insert;
                    createdA.Quantity = current.Quantity;

                    pair_occurrence createdB = {};
                    createdB.Pair[0] = rule.Insert;
                    createdB.Pair[1] = current.Pair[1];
                    createdB.Quantity = current.Quantity;

                    (buffer + pairIndex)->Quantity -= current.Quantity;

                    u8 pairs = 2;
                    for (u32 bufferIndex = 0; bufferIndex < ruleCount; bufferIndex++)
                    {
                        if ((buffer + bufferIndex)->Pair[0] == createdA.Pair[0] && (buffer + bufferIndex)->Pair[1] == createdA.Pair[1])
                        {
                            (buffer + bufferIndex)->Quantity += createdA.Quantity;
                            pairs--;
                        }

                        if ((buffer + bufferIndex)->Pair[0] == createdB.Pair[0] && (buffer + bufferIndex)->Pair[1] == createdB.Pair[1])
                        {
                            (buffer + bufferIndex)->Quantity += createdB.Quantity;
                            pairs--;
                        }

                        if (pairs == 0) break;
                    }

                    break;
                }
            }
        }
    }

    free(pairs);
    return buffer;
}

internal u64
Part1()
{
    file_data file = ReadToEndOfFile("input\\day14-input.txt");
    parse_result parseResult = Parse(file);

    pair_occurrence *occurrences = parseResult.Occurrences;

    for (u32 count = 0; count < 10; count++)
        occurrences = Sequence(occurrences, parseResult.Rules, parseResult.RuleCount);

    u64 result = Analyze(occurrences, parseResult.RuleCount, parseResult.LastElement);
    
    return result;
}

internal u64
Part2()
{
    file_data file = ReadToEndOfFile("input\\day14-input.txt");
    parse_result parseResult = Parse(file);

    pair_occurrence *occurrences = parseResult.Occurrences;

    for (u32 count = 0; count < 40; count++)
        occurrences = Sequence(occurrences, parseResult.Rules, parseResult.RuleCount);

    u64 result = Analyze(occurrences, parseResult.RuleCount, parseResult.LastElement);

    return result;
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

    DebugLog("- Day 14 -\n");
    DebugLog("Result Part 1: %lld (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %lld (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}