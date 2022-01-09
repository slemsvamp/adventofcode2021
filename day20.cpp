#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char *NEW_LINE = "\r\n";

const s32 _universe_side = 46000;
const s32 _verticalDirection[9] = {-1,-1,-1,0,0,0,1,1,1};
const s32 _horizontalDirection[9] = {-1,0,1,-1,0,1,-1,0,1};
const u32 _maxLightPixels = 96000;

struct size
{
    u32 Width;
    u32 Height;
};

struct coordinate
{
    u32 X;
    u32 Y;
};

struct parse_result
{
    char *EnhancementAlgorithm;
    u32 AlgorithmLength;
    
    u64 *LightPixels;
    u32 Count;
    
    coordinate ImagePosition;
    size ImageSize;
};

struct scan_result
{
    u64 *LightPixels;
    u32 Count;
    coordinate ImagePosition;
    size ImageSize;
};

internal coordinate
CreateCoordinate(u32 y, u32 x)
{
    coordinate coord = {};
    coord.X = x;
    coord.Y = y;
    return coord;
}

internal u64
CoordinateToU64(coordinate coord)
{
    return (u64)coord.Y * _universe_side + coord.X;
}

internal coordinate
U64ToCoordinate(u64 hash)
{
    coordinate result = {};
    result.Y = hash / _universe_side;
    result.X = hash % _universe_side;
    return result;
}

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    char *readBuffer = (char *)malloc(file.Size);
    memcpy(readBuffer, file.Data, file.Size);

    char *line = strtok(readBuffer, NEW_LINE);

    result.AlgorithmLength = strlen(line);
    result.EnhancementAlgorithm = (char *)calloc(strlen(line), sizeof(char));
    memcpy(result.EnhancementAlgorithm, line, strlen(line));

    line = strtok(NULL, NEW_LINE);

    u32 width = strlen(line);
    u32 height = 0;

    u64 *buffer = (u64 *)calloc(_maxLightPixels, sizeof(u64));
    u32 count = 0;

    result.ImagePosition.X = _universe_side / 2;
    result.ImagePosition.Y = _universe_side / 2;

    while (line)
    {
        for (u32 lineIndex = 0; lineIndex < width; lineIndex++)
            if (*(line + lineIndex) == '#')
            {
                coordinate c = {};
                c.Y = result.ImagePosition.Y + height;
                c.X = result.ImagePosition.X + lineIndex;
                *(buffer + count++) = CoordinateToU64(c);
                Assert(count < _maxLightPixels);
            }

        height++;
        line = strtok(NULL, NEW_LINE);
    }

    result.ImageSize.Width = width;
    result.ImageSize.Height = height;
    result.Count = count;
    result.LightPixels = (u64 *)malloc(count * sizeof(u64));
    memcpy(result.LightPixels, buffer, count * sizeof(u64));

    free(buffer);
    free(readBuffer);

    return result;
}

internal void
PrintImage(coordinate imagePosition, size imageSize, u32 count, u64 *lightPixels)
{
    DebugLog("----- Scan -----\n");
    DebugLog("ImagePosition Y%d/X%d\n", imagePosition.Y, imagePosition.X);
    DebugLog("ImageSize H%d/W%d\n", imageSize.Height, imageSize.Width);
    DebugLog("Count %d\n", count);

    char *debugPrint = (char *)malloc(imageSize.Width * imageSize.Height * sizeof(char));
    for (u32 resetIndex = 0; resetIndex < imageSize.Width * imageSize.Height; resetIndex++)
        *(debugPrint + resetIndex) = '.';

    for (u32 lightPixelIndex = 0; lightPixelIndex < count; lightPixelIndex++)
    {
        u64 hash = *(lightPixels + lightPixelIndex);
        coordinate c = U64ToCoordinate(hash);
        c.X -= imagePosition.X;
        c.Y -= imagePosition.Y;
        *(debugPrint + c.Y * imageSize.Width + c.X) = '#';
    }

    char formatString[10] = "";
    char widthString[10];
    strcat(formatString, "%.");
    itoa(imageSize.Width, widthString, 10);
    strcat(formatString, widthString);
    strcat(formatString, "s%s");

    for (u32 y = 0; y < imageSize.Height; y++)
        DebugLog(formatString, debugPrint + y * imageSize.Width, "\n");

    DebugLog("- %s ------------------\n", formatString);
}

internal scan_result
Scan(coordinate imagePosition, size imageSize, u64 *lightPixels, u32 count, char *enhancementAlgorithm, b8 on)
{
    scan_result result = {};

    u64 *lightPixelBuffer = (u64 *)malloc(_maxLightPixels * sizeof(u64));
    u64 bufferCount = 0;

    coordinate min = imagePosition;
    coordinate max = CreateCoordinate(imagePosition.Y + imageSize.Height - 1, imagePosition.X + imageSize.Width - 1);

    s32 lowModifier = !on ? -3 : 1;
    s32 highModifier = !on ? +3 : -1;

    for (u32 scanY = imagePosition.Y + lowModifier; scanY < imagePosition.Y + imageSize.Height + highModifier; scanY++)
        for (u32 scanX = imagePosition.X + lowModifier; scanX < imagePosition.X + imageSize.Width + highModifier; scanX++)
        {
            coordinate scan = {};
            scan.Y = scanY;
            scan.X = scanX;

            u32 magnitude = 8;
            u32 algorithmLookup = 0;

            char tinyBuffer[9] = { '0', '0', '0', '0', '0', '0', '0', '0', '0' };

            for (u32 subScanIndex = 0; subScanIndex < 9; subScanIndex++)
            {
                coordinate subScan = {};
                subScan.Y = scan.Y + *(_verticalDirection + subScanIndex);
                subScan.X = scan.X + *(_horizontalDirection + subScanIndex);
                u64 hash = CoordinateToU64(subScan);
                
                Assert(magnitude >= 0 && magnitude <= 8);

                if (Contains(lightPixels, count, hash))
                {
                    tinyBuffer[subScanIndex] = '1';
                    algorithmLookup += (1 << magnitude);
                }
                
                magnitude--;
            }

            Assert(algorithmLookup < 512);

            char pixel = *(enhancementAlgorithm + algorithmLookup);
            
            if (pixel == '#')
            {
                u64 scanHash = CoordinateToU64(scan);

                *(lightPixelBuffer + bufferCount++) = scanHash;

                Assert(bufferCount < _maxLightPixels);
                
                if (scan.X < min.X) min.X = scan.X;
                if (scan.X > max.X) max.X = scan.X;
                if (scan.Y < min.Y) min.Y = scan.Y;
                if (scan.Y > max.Y) max.Y = scan.Y;
            }
        }
        
    result.Count = bufferCount;
    result.LightPixels = (u64 *)malloc(bufferCount * sizeof(u64));
    memcpy(result.LightPixels, lightPixelBuffer, bufferCount * sizeof(u64));
    result.ImagePosition.X = min.X;
    result.ImagePosition.Y = min.Y;
    result.ImageSize.Width = max.X - min.X + 1;
    result.ImageSize.Height = max.Y - min.Y + 1;

    free(lightPixelBuffer);
    return result;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day20-input.txt");
    parse_result parseResult = Parse(file);

    coordinate imagePosition = parseResult.ImagePosition;
    size imageSize = parseResult.ImageSize;
    u64 *lightPixels = parseResult.LightPixels;
    u64 count = parseResult.Count;

    u32 runs = 2;

    for (u32 runIndex = 0; runIndex < runs; runIndex++)
    {
        scan_result scanResult = Scan(imagePosition, imageSize, lightPixels, count, parseResult.EnhancementAlgorithm, runIndex % 2 == 1);
        
        free(lightPixels);
        
        imagePosition = scanResult.ImagePosition;
        imageSize = scanResult.ImageSize;
        lightPixels = scanResult.LightPixels;
        count = scanResult.Count;
    }

    return count;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day20-input.txt");
    parse_result parseResult = Parse(file);

    coordinate imagePosition = parseResult.ImagePosition;
    size imageSize = parseResult.ImageSize;
    u64 *lightPixels = parseResult.LightPixels;
    u64 count = parseResult.Count;

    u32 runs = 50;

    for (u32 runIndex = 0; runIndex < runs; runIndex++)
    {
        scan_result scanResult = Scan(imagePosition, imageSize, lightPixels, count, parseResult.EnhancementAlgorithm, runIndex % 2 == 1);
        
        free(lightPixels);
        
        imagePosition = scanResult.ImagePosition;
        imageSize = scanResult.ImageSize;
        lightPixels = scanResult.LightPixels;
        count = scanResult.Count;
    }

    return count;
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

    DebugLog("- Day 20 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}