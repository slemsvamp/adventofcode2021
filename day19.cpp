#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define MAX_SCANNER_BUFFER 100
#define MAX_BEACON_BUFFER 100
#define MAX_SCANNER_MATCH_BUFFER 1000
#define MAX_BEACON_MATCH_BUFFER 1000
#define MIN_BEACONS_TO_MATCH 3

const char *NEW_LINE = "\r\n";

//const u8 _rotationsOfX[24] = {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3};
//const u8 _rotationsOfY[24] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const u8 _rotationsOfZ[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3};

const u8 _rotationsOfX[24] = {0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 1, 2, 3, 1, 1, 1, 1, 3, 3, 3, 3};
const u8 _rotationsOfY[24] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 1, 2, 3, 0, 1, 2, 3};
const u8 _rotationsOfZ[24] = {0, 1, 2, 3, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//const u8 _normalizeRotationsOfX[24] = {0, 3, 2, 1, 0, 3, 2, 1, 3, 2, 1, 3, 2, 1, 0, 3, 2, 1, 3, 2, 1, 3, 2, 1};
//const u8 _normalizeRotationsOfY[24] = {0, 0, 0, 0, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const u8 _normalizeRotationsOfZ[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1};

const u8 _normalizeRotationsOfX[24] = {0, 0, 0, 0, 0, 3, 2, 1, 0, 0, 0, 0, 0, 3, 2, 1, 3, 3, 3, 3, 1, 1, 1, 1};
const u8 _normalizeRotationsOfY[24] = {0, 0, 0, 0, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 3, 2, 1, 0, 3, 2, 1};
const u8 _normalizeRotationsOfZ[24] = {0, 3, 2, 1, 0, 0, 0, 0, 0, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct coordinate
{
    s32 X;
    s32 Y;
    s32 Z;
};

inline coordinate
operator+(coordinate left, coordinate right)
{
    coordinate result = {};
    result.X = left.X + right.X;
    result.Y = left.Y + right.Y;
    result.Z = left.Z + right.Z;
    return result;
}

inline coordinate
operator-(coordinate left, coordinate right)
{
    coordinate result = {};
    result.X = left.X - right.X;
    result.Y = left.Y - right.Y;
    result.Z = left.Z - right.Z;
    return result;
}

inline b8
operator==(coordinate left, coordinate right)
{
    return left.X == right.X && left.Y == right.Y && left.Z == right.Z;
}

internal char*
CoordinateToKey(coordinate c)
{
    char *result = (char *)calloc(40, sizeof(char));
    char *buffer = (char *)calloc(10, sizeof(char));
    itoa(c.X, buffer, 10);
    strcat(result, buffer);
    strcat(result, "x");
    memset(buffer, 0, 10);
    itoa(c.Y, buffer, 10);
    strcat(result, buffer);
    strcat(result, "x");
    memset(buffer, 0, 10);
    itoa(c.Z, buffer, 10);
    strcat(result, buffer);
    strcat(result, "\0");
    free(buffer);
    return result;
}

internal matrix
MatrixTranslation(coordinate coord)
{
    return MatrixTranslation(coord.X, coord.Y, coord.Z);
}

internal coordinate
CoordinateFromMatrix(matrix m)
{
    coordinate result = {};
    result.X = m.M30;
    result.Y = m.M31;
    result.Z = m.M32;
    return result;
}

struct scanner
{
    coordinate Coordinate;
    coordinate *Beacons;
    u8 BeaconCount;
};

struct parse_result
{
    scanner *Scanners;
    u16 ScannerCount;
};

internal void
SkipPast(char **buffer, char character)
{
    char *playhead = *buffer;
    while (*playhead != character)
        playhead++;
    *buffer = playhead + 1;
}

internal void
Parse_AddScanner(u16 *beaconCount, coordinate **beaconBuffer, u16 *scannerCount, scanner **scannerBuffer, scanner *currentScanner)
{
    currentScanner->BeaconCount = *beaconCount;
    currentScanner->Beacons = (coordinate *)malloc(*beaconCount * sizeof(coordinate));
    memcpy(currentScanner->Beacons, *beaconBuffer, *beaconCount * sizeof(coordinate));

    *(*scannerBuffer + (*scannerCount)++) = *currentScanner;

    *beaconCount = 0;
    *currentScanner = {};
}

internal parse_result
Parse(file_data file)
{
    parse_result result = {};

    char *readBuffer = (char *)malloc(file.Size);
    memcpy(readBuffer, file.Data, file.Size);

    char *line = strtok(readBuffer, NEW_LINE);

    scanner *scannerBuffer = (scanner *)calloc(MAX_SCANNER_BUFFER, sizeof(scanner));
    u16 scannerCount = 0;
    coordinate *beaconBuffer = (coordinate *)calloc(MAX_BEACON_BUFFER, sizeof(coordinate));
    u16 beaconCount = 0;

    scanner currentScanner = {};
    b8 first = true;

    while (line)
    {
        if (strstr(line, "--- scanner") != NULL)
        {
            if (!first)
                Parse_AddScanner(&beaconCount, &beaconBuffer, &scannerCount, &scannerBuffer, &currentScanner);
            first = false;
        }
        else
        {
            char *skipAhead = line;
            coordinate beacon = {};
            beacon.X = atoi(skipAhead);
            SkipPast(&skipAhead, ',');
            beacon.Y = atoi(skipAhead);
            SkipPast(&skipAhead, ',');
            beacon.Z = atoi(skipAhead);
            *(beaconBuffer + beaconCount++) = beacon;
        }

        line = strtok(NULL, NEW_LINE);
    }

    Parse_AddScanner(&beaconCount, &beaconBuffer, &scannerCount, &scannerBuffer, &currentScanner);

    result.ScannerCount = scannerCount;
    result.Scanners = (scanner *)malloc(scannerCount * sizeof(scanner));
    memcpy(result.Scanners, scannerBuffer, scannerCount * sizeof(scanner));

    free(beaconBuffer);
    free(scannerBuffer);

    return result;
}

struct scanner_match
{
    u32 ScannerIndex;
    u32 RotationIndex;
    coordinate *Beacons;
    u32 BeaconMatchCount;
    coordinate ScannerRelativePosition;
};

struct match_result
{
    scanner_match *ScannerMatches;
    u32 ScannerMatchCount;
};

inline matrix
RotateMatrix(u32 rotationsX, u32 rotationsY, u32 rotationsZ) //, matrix matcheeBeaconMatrix)
{
    matrix result = MatrixIdentity();
    for (u32 rotations = rotationsX; rotations > 0; rotations--)
        result = result * MatrixRotationX();
    for (u32 rotations = rotationsY; rotations > 0; rotations--)
        result = result * MatrixRotationY();
    for (u32 rotations = rotationsZ; rotations > 0; rotations--)
        result = result * MatrixRotationZ();
    return result;
}

inline matrix
UnrotateMatrix(u32 rotationsX, u32 rotationsY, u32 rotationsZ) //, matrix matcheeBeaconMatrix)
{
    matrix result = MatrixIdentity();
    for (u32 rotations = rotationsZ; rotations > 0; rotations--)
        result = result * MatrixRotationZ();
    for (u32 rotations = rotationsY; rotations > 0; rotations--)
        result = result * MatrixRotationY();
    for (u32 rotations = rotationsX; rotations > 0; rotations--)
        result = result * MatrixRotationX();
    return result;
}

internal scanner_match
MatchWith(u32 matcherIndex, u32 matcheeIndex, parse_result parseResult)
{
    scanner_match result = {};

    if (matcheeIndex == matcherIndex)
        return {};

    // scanner_match *scannerMatchBuffer = (scanner_match *)malloc(MAX_SCANNER_MATCH_BUFFER * sizeof(scanner_match));
    // u32 scannerMatchCount = 0;

    result.ScannerIndex = matcheeIndex;

    coordinate *beaconMatchBuffer = (coordinate *)malloc(MAX_BEACON_MATCH_BUFFER * sizeof(coordinate));

    scanner matcher = *(parseResult.Scanners + matcherIndex);
    scanner matchee = *(parseResult.Scanners + matcheeIndex);

    matrix matcherMatrix = MatrixIdentity();

    // we lock in one of many rotations we need to check
    for (u32 rotationIndex = 0; rotationIndex < 24; rotationIndex++)
    {
        u32 rotationsX = _rotationsOfX[rotationIndex];
        u32 rotationsY = _rotationsOfY[rotationIndex];
        u32 rotationsZ = _rotationsOfZ[rotationIndex];
        matrix rotationMatrix = RotateMatrix(rotationsX, rotationsY, rotationsZ);

        // we pick a beacon from the matcher scanner to cross-reference
        for (u32 referenceMatcherBeaconIndex = 0; referenceMatcherBeaconIndex < matchee.BeaconCount; referenceMatcherBeaconIndex++)
        {
            coordinate referenceMatcherBeacon = *(matcher.Beacons + referenceMatcherBeaconIndex);

            // we pick a beacon from the matchee scanner to cross-reference
            for (u32 referenceMatcheeBeaconIndex = 0; referenceMatcheeBeaconIndex < matchee.BeaconCount; referenceMatcheeBeaconIndex++)
            {
                coordinate referenceMatcheeBeacon = *(matchee.Beacons + referenceMatcheeBeaconIndex);

                matrix referenceMatcheeBeaconMatrix = MatrixTranslation(referenceMatcheeBeacon) * rotationMatrix;
                coordinate rotatedReferenceMatcheeBeaconCoordinate = CoordinateFromMatrix(referenceMatcheeBeaconMatrix);

                coordinate proposedMatcheeScannerCoordinate = referenceMatcherBeacon - rotatedReferenceMatcheeBeaconCoordinate;

                u32 matches = 0;

                for (u32 matcheeBeaconIndex = 0; matcheeBeaconIndex < matchee.BeaconCount; matcheeBeaconIndex++)
                {
                    coordinate matcheeBeacon = *(matchee.Beacons + matcheeBeaconIndex);

                    matrix matcheeBeaconMatrix = MatrixTranslation(matcheeBeacon) * rotationMatrix;
                    coordinate rotatedMatcheeBeaconCoordinate = CoordinateFromMatrix(matcheeBeaconMatrix);

                    for (u32 matcherBeaconIndex = 0; matcherBeaconIndex < matcher.BeaconCount; matcherBeaconIndex++)
                    {
                        coordinate matcherBeacon = *(matcher.Beacons + matcherBeaconIndex);

                        coordinate matcheeProposedCoordinate = {};
                        matcheeProposedCoordinate = matcherBeacon - rotatedMatcheeBeaconCoordinate;

                        // check if there are matches with the matcher beacons
                        if (proposedMatcheeScannerCoordinate == matcheeProposedCoordinate)
                        {
                            *(beaconMatchBuffer + matches++) = matcherBeacon;

                            Assert(matches < MAX_BEACON_MATCH_BUFFER);
                        }
                    }
                }

                if (matches >= MIN_BEACONS_TO_MATCH)
                {
                    result.RotationIndex = rotationIndex;
                    result.ScannerRelativePosition = proposedMatcheeScannerCoordinate;
                    result.BeaconMatchCount = matches;
                    result.Beacons = (coordinate *)malloc(matches * sizeof(coordinate));
                    memcpy(result.Beacons, beaconMatchBuffer, matches * sizeof(coordinate));

                    free(beaconMatchBuffer);

                    return result;
                }
            }
        }
    }

    return {};
}

struct scanner_matches
{
    scanner_match *Matches;
    u32 Count;
};

/*
internal scanner_matches
GetScannerMatches(u32 matcherIndex, s32 *scanners, parse_result parseResult)
{
    scanner_matches result = {};

    *(scanners + matcherIndex) = -1;

    scanner_match *scannerMatchBuffer = (scanner_match *)malloc(parseResult.ScannerCount * sizeof(scanner_match));
    u32 count = 0;

    for (u32 scannerIndex = 0; scannerIndex < parseResult.ScannerCount; scannerIndex++)
    {
        u32 matcheeIndex = *(scanners + scannerIndex);
        
        if (matcheeIndex < 0)
            continue;

        scanner_match scannerMatch = MatchWith(matcherIndex, matcheeIndex, parseResult);

        if (scannerMatch.BeaconMatchCount > 0)
            *(scannerMatchBuffer + count++) = scannerMatch;
    }

    result.Count = count;
    result.Matches = (scanner_match *)malloc(count * sizeof(scanner_match));
    memcpy(result.Matches, scannerMatchBuffer, count * sizeof(scanner_match));

    return result;
}
*/

struct detection_history
{
    coordinate RelativePosition;
    u32 RotationIndex;
};

s32 _originScannerIndex = -1;

internal void
GetScannerMatchesRecursive(u32 matcherIndex, s32 *scanners, parse_result parseResult, dictionary *beaconDictionary, detection_history *detectionHistory, u32 detectionHistoryCount)
{
    scanner_matches result = {};

    *(scanners + matcherIndex) = -1;

    scanner_match *scannerMatchBuffer = (scanner_match *)malloc(parseResult.ScannerCount * sizeof(scanner_match));
    u32 count = 0;

    for (u32 scannerIndex = 0; scannerIndex < parseResult.ScannerCount; scannerIndex++)
    {
        s32 matcheeIndex = *(scanners + scannerIndex);
        
        if (matcheeIndex < 0)
            continue;

        scanner_match scannerMatch = MatchWith(matcherIndex, matcheeIndex, parseResult);

        if (scannerMatch.BeaconMatchCount >= MIN_BEACONS_TO_MATCH)
        {
            DebugLog("MATCHED (%d) [Scanner %d]->[Scanner %d] at Rotation %d\n", scannerMatch.BeaconMatchCount, matcherIndex, matcheeIndex, scannerMatch.RotationIndex);
            DebugLog("Relative Position: %d,%d,%d\n", scannerMatch.ScannerRelativePosition.X, scannerMatch.ScannerRelativePosition.Y, scannerMatch.ScannerRelativePosition.Z);
            DebugLog("------------------------------------------\n");

            scanner matcherScanner = *(parseResult.Scanners + matcherIndex);
            scanner matcheeScanner = *(parseResult.Scanners + matcheeIndex);

            detection_history *detectionHistoryBuffer = (detection_history *)calloc(detectionHistoryCount * 2 + 2, sizeof(detection_history));
            memcpy(detectionHistoryBuffer, detectionHistory, detectionHistoryCount * sizeof(detection_history));


            // 1. Put all the matchee beacons in a buffer so we can change them
            // 2. Iterate through detectionHistory
            //   2.1. Rotate the relative position of scanner
            //   2.2. Rotate all beacons
            //   2.3. Apply relative coordinate?

            // So the idea is to go one scanner back at a time and keep beacons relative until we get to the origin


            if (_originScannerIndex < 0)
            {
                _originScannerIndex = matcherIndex;
                *(detectionHistoryBuffer + detectionHistoryCount++) = {0, 0, 0, 0};

                // The first scanner we match we'll consider being the origin scanner and thus we only
                // add these beacons relative position as this scanner is seen as being the origin point of 0,0,0

                for (u32 beaconIndex = 0; beaconIndex < matcherScanner.BeaconCount; beaconIndex++)
                {
                    coordinate beacon = *(matcherScanner.Beacons + beaconIndex);
                    char *key = CoordinateToKey(beacon);

                    DebugLog("Found '%s', ", key);

                    if (!DICT_ContainsKey(beaconDictionary, key))
                    {
                        DICT_Add(beaconDictionary, key, (void *)(matcherScanner.Beacons + beaconIndex));
                        DebugLog("added.\n");
                    }
                    else DebugLog("already existed.\n");
                }
            }

            matrix rotateMatrix = MatrixIdentity();
            coordinate coordinateAdjustment = {};

            detection_history historyEntry = {};
            historyEntry.RotationIndex = scannerMatch.RotationIndex;
            historyEntry.RelativePosition = scannerMatch.ScannerRelativePosition;
            *(detectionHistoryBuffer + detectionHistoryCount++) = historyEntry;

            // This is just a debug print of what's in the detectionHistoryBuffer
            for (u32 historyIndex = 0; historyIndex < detectionHistoryCount; historyIndex++)
            {
                detection_history historyEntry = *(detectionHistoryBuffer + historyIndex);
                DebugLog("Entry (X=%d, Y=%d, Z=%d) RI=%d\n", historyEntry.RelativePosition.X, historyEntry.RelativePosition.Y, historyEntry.RelativePosition.Z, historyEntry.RotationIndex);
            }

            // add matcher beacons with current history
            for (s32 historyIndex = detectionHistoryCount - 1; historyIndex > 0; historyIndex--)
            {
                detection_history historyEntry = *(detectionHistoryBuffer + historyIndex - 1);
                rotateMatrix = rotateMatrix * RotateMatrix(_rotationsOfX[historyEntry.RotationIndex], _rotationsOfY[historyEntry.RotationIndex], _rotationsOfZ[historyEntry.RotationIndex]);
                detection_history historyEntry2 = *(detectionHistoryBuffer + historyIndex);
                matrix m = MatrixTranslation(historyEntry2.RelativePosition) * rotateMatrix;
                coordinate c = CoordinateFromMatrix(m);

                //if (historyIndex != detectionHistoryCount - 1)
                    coordinateAdjustment = coordinateAdjustment + c;

                // TODO: We need to rotate the whole coordinateAdjustment after adding the relative
                DebugLog("Adjustment: (X=%d, Y=%d, Z=%d)\n", c.X, c.Y, c.Z);
            }

            DebugLog("Coordinate Adjustment: (X=%d, Y=%d, Z=%d)\n", coordinateAdjustment.X, coordinateAdjustment.Y, coordinateAdjustment.Z);
            DebugLog("ScannerMatch RelativePosition: (X=%d, Y=%d, Z=%d)\n", scannerMatch.ScannerRelativePosition.X, scannerMatch.ScannerRelativePosition.Y, scannerMatch.ScannerRelativePosition.Z);
            //coordinateAdjustment = coordinateAdjustment + scannerMatch.ScannerRelativePosition;
            //DebugLog("Coordinate Adjustment: (X=%d, Y=%d, Z=%d)\n", coordinateAdjustment.X, coordinateAdjustment.Y, coordinateAdjustment.Z);

            detection_history lastEntry = *(detectionHistoryBuffer + detectionHistoryCount - 1);
            rotateMatrix = rotateMatrix * RotateMatrix(_rotationsOfX[lastEntry.RotationIndex], _rotationsOfY[lastEntry.RotationIndex], _rotationsOfZ[lastEntry.RotationIndex]);

            for (u32 beaconIndex = 0; beaconIndex < matcheeScanner.BeaconCount; beaconIndex++)
            {
                coordinate beacon = *(matcheeScanner.Beacons + beaconIndex);
                //if (beacon.Z > 0) DebugLog("Special A: (X=%d, Y=%d, Z=%d)\n", beacon.X, beacon.Y, beacon.Z);
                matrix rotatedBeaconMatrix = MatrixTranslation(beacon) * rotateMatrix;
                coordinate *rotatedBeacon = (coordinate *)malloc(sizeof(coordinate));
                *rotatedBeacon = CoordinateFromMatrix(rotatedBeaconMatrix);

                //if (beacon.Z > 0) DebugLog("Special B: (X=%d, Y=%d, Z=%d)\n", rotatedBeacon->X, rotatedBeacon->Y, rotatedBeacon->Z);

                *rotatedBeacon = *rotatedBeacon + coordinateAdjustment;

                //if (beacon.Z > 0) DebugLog("Special C: (X=%d, Y=%d, Z=%d)\n", rotatedBeacon->X, rotatedBeacon->Y, rotatedBeacon->Z);

                char *key = CoordinateToKey(*rotatedBeacon);

                //DebugLog("Found '%s', ", key);

                if (!DICT_ContainsKey(beaconDictionary, key))
                {
                    DICT_Add(beaconDictionary, key, rotatedBeacon);
                    //DebugLog("added.\n");
                }
                else
                {
                    //DebugLog("already existed.\n");
                }
            }

            /*
            if (detectionHistoryCount > 0)
            {
                coordinateAdjustment = (*detectionHistory).DetectedByCoordinate;
            }

            for (u32 historyIndex = 0; historyIndex < detectionHistoryCount; historyIndex++)
            {
                detection_history historyEntry = *(detectionHistory + historyIndex);
                unrotateMatrix = unrotateMatrix * UnrotateMatrix(_normalizeRotationsOfX[historyEntry.RotationIndex], _normalizeRotationsOfY[historyEntry.RotationIndex], _normalizeRotationsOfZ[historyEntry.RotationIndex]);
                matrix scannerMatrix = MatrixTranslation(scannerMatch.ScannerRelativePosition) * unrotateMatrix;
                coordinate rotatedScannerPosition = CoordinateFromMatrix(scannerMatrix);
                DebugLog("Corrected Position (X=%d, Y=%d, Z=%d)\n", rotatedScannerPosition.X, rotatedScannerPosition.Y, rotatedScannerPosition.Z);
                //coordinateAdjustment -= historyEntry.DetectedByCoordinate;
            }

            for (u32 historyIndex = 0; historyIndex < detectionHistoryCount; historyIndex++)
            {
                detection_history historyEntry = *(detectionHistory + historyIndex);
                DebugLog("Entry (X=%d, Y=%d, Z=%d) RI=%d\n", historyEntry.DetectedByCoordinate.X, historyEntry.DetectedByCoordinate.Y, historyEntry.DetectedByCoordinate.Z, historyEntry.RotationIndex);
            }
            */

            // we are only looking at the matcher beacons, when will we add the matchee beacons then?

            /*
            for (u32 beaconIndex = 0; beaconIndex < matcherScanner.BeaconCount; beaconIndex++)
            {
                coordinate beacon = *(matcherScanner.Beacons + beaconIndex);
                matrix rotatedBeaconMatrix = MatrixTranslation(beacon) * unrotateMatrix;
                coordinate *rotatedBeacon = (coordinate *)malloc(sizeof(coordinate));
                *rotatedBeacon = CoordinateFromMatrix(rotatedBeaconMatrix) + coordinateAdjustment;
                char *key = CoordinateToKey(*rotatedBeacon);

                DebugLog("Found '%s', ", key);

                if (!DICT_ContainsKey(beaconDictionary, key))
                {
                    DICT_Add(beaconDictionary, key, rotatedBeacon);
                    DebugLog("added.\n");
                }
                else
                {
                    DebugLog("already existed.\n");
                }
            }

            detection_history *newDetectionHistory = (detection_history *)malloc((detectionHistoryCount + 1) * sizeof(detection_history));
            memcpy(newDetectionHistory, detectionHistory, detectionHistoryCount * sizeof(detection_history));

            detection_history newEntry = {};
            newEntry.RotationIndex = scannerMatch.RotationIndex;
            newEntry.RelativePosition = scannerMatch.ScannerRelativePosition;
            */
/*
            // TODO: bleh
            matrix x = MatrixTranslation(scannerMatch.ScannerRelativePosition);
            x = x * unrotateMatrix; //RotateMatrix(_rotationsOfX[scannerMatch.RotationIndex], _rotationsOfY[scannerMatch.RotationIndex], _rotationsOfZ[scannerMatch.RotationIndex]);
            coordinate xyz = CoordinateFromMatrix(x) + coordinateAdjustment;

            DebugLog("AdjustmentCoordinate: X%d, Y%d, Z%d\n", xyz.X, xyz.Y, xyz.Z);
*/


            *(scanners + matcheeIndex) = -1;

            GetScannerMatchesRecursive(matcheeIndex, scanners, parseResult, beaconDictionary, detectionHistoryBuffer, detectionHistoryCount);
        }
        else
        {
            DebugLog("FAILED [Scanner %d]->[Scanner %d]\n", matcherIndex, matcheeIndex);
            DebugLog("------------------------------------------\n");
        }
    }
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\temp.txt");
    parse_result parseResult = Parse(file);

    // TODO: start with some scanner, collect a list of matches
    // then go through (foreach in) list of matches and collect their matches
    // until no more matches have been made, along the way we should be able to keep
    // the rotation matrix for each step since we're following the
    // chains of detection.

    // Start at matcherIndex == 0 and iterate
    // As soon as MatchWith returns matches for 1 or more scanners:
    // - we add the correctly oriented beacons to a masterlist
    // - we iterate through the scanners we got back knowing current rotation
    // We need to keep a list of what scanners we've used as a source already so that we do not re-use them (no circular references)

    dictionary beaconDictionary = {};
    DICT_Init(&beaconDictionary, 10000);

    s32 *scanners = (s32 *)malloc(parseResult.ScannerCount * sizeof(s32));
    for (u32 scannerIndex = 0; scannerIndex < parseResult.ScannerCount; scannerIndex++)
        *(scanners + scannerIndex) = scannerIndex;

    detection_history *detectionHistory = (detection_history *)malloc(parseResult.ScannerCount * sizeof(detection_history));
    u32 detectionHistoryCount = 0;

    for (u32 matcherIndex = 0; matcherIndex < parseResult.ScannerCount; matcherIndex++)
    {
        GetScannerMatchesRecursive(matcherIndex, scanners, parseResult, &beaconDictionary, detectionHistory, detectionHistoryCount);
    }

    // we could move the scannerMatchBuffer to a fixed size result if we want, but we'll only at most keep something like 30-40 anyway.

    // remove matcher from possible scanners to check
    
    // remove matchees (list) from possible scanners to check so we don't perform unnecessary scanner checks

    // recursively check through (MatchWith) each matcheeId in the list against scanners that are left

    // we keep track of rotation indices as we go, so that we can rotate beacons back to the origin

    // we can add these to a dictionary, if we do a get check first

    /*
    for (u32 matcherIndex = 0; matcherIndex < parseResult.ScannerCount; matcherIndex++)
        for (u32 matcheeIndex = matcherIndex + 1; matcheeIndex < parseResult.ScannerCount; matcheeIndex++)
        {
            if (matcherIndex == matcheeIndex)
                continue;

            scanner_match scannerMatch = MatchWith(matcherIndex, matcheeIndex, parseResult);

            if (scannerMatch.BeaconMatchCount > 0)
            {
                (parseResult.Scanners + matcheeIndex)->Coordinate = scannerMatch.ScannerRelativePosition;

                DebugLog("MATCHED (%d) [Scanner %d]->[Scanner %d] at Rotation %d\n", scannerMatch.BeaconMatchCount, matcherIndex, matcheeIndex, scannerMatch.RotationIndex);
                DebugLog("Relative Position: %d,%d,%d\n", scannerMatch.ScannerRelativePosition.X, scannerMatch.ScannerRelativePosition.Y, scannerMatch.ScannerRelativePosition.Z);
                DebugLog("------------------------------------------\n");
            }

            free(scannerMatch.Beacons);
        }
    */

    /*
    for (u32 scannerIndex = 0; scannerIndex < parseResult.ScannerCount; scannerIndex++)
    {
        scanner *currentScanner = (parseResult.Scanners + scannerIndex);

        if (*(*(detectedBy + scannerIndex)) == 0xffffffff)
        {
            // not detected by anyone
        }
    }

    scanner scanner0 = *(parseResult.Scanners);
    scanner scanner1 = *(parseResult.Scanners + 1);
    scanner scanner2 = *(parseResult.Scanners + 2);
    scanner scanner3 = *(parseResult.Scanners + 3);
    scanner scanner4 = *(parseResult.Scanners + 4);

    matrix unrotateMatrix1 = UnrotateMatrix(_normalizeRotationsOfX[scanner0.RotationIndex], _normalizeRotationsOfY[scanner0.RotationIndex], _normalizeRotationsOfZ[scanner0.RotationIndex]);
    matrix unrotateMatrix3 = UnrotateMatrix(_normalizeRotationsOfX[scanner1.RotationIndex], _normalizeRotationsOfY[scanner1.RotationIndex], _normalizeRotationsOfZ[scanner1.RotationIndex]);

    matrix scanner1Matrix = MatrixTranslation(scanner1.Coordinate);
    matrix scanner3Matrix = MatrixTranslation(scanner3.Coordinate);

    scanner1Matrix = scanner1Matrix * unrotateMatrix1;
    scanner3Matrix = scanner3Matrix * unrotateMatrix1 * unrotateMatrix3;

    coordinate scanner1Coordinate = CoordinateFromMatrix(scanner1Matrix);
    coordinate scanner3Coordinate = CoordinateFromMatrix(scanner3Matrix);

    DebugLog("scanner1: %d,%d,%d\n", scanner1Coordinate.X, scanner1Coordinate.Y, scanner1Coordinate.Z);
    DebugLog("scanner3: %d,%d,%d\n", scanner3Coordinate.X, scanner3Coordinate.Y, scanner3Coordinate.Z);

    coordinate rotatedScanner3Coordinate = CoordinateFromMatrix(scanner1Matrix) + CoordinateFromMatrix(scanner3Matrix);

    DebugLog("rotated: %d,%d,%d\n", rotatedScanner3Coordinate.X, rotatedScanner3Coordinate.Y, rotatedScanner3Coordinate.Z);
    */

    return beaconDictionary.Adds;
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\temp.txt");

    /*
    coordinate c = {-68,1246,43};
    for (u32 rotationIndex = 0; rotationIndex < 24; rotationIndex++)
    {
        matrix m = MatrixTranslation(c);
        matrix rotationMatrix = UnrotateMatrix(_normalizeRotationsOfX[rotationIndex], _normalizeRotationsOfY[rotationIndex], _normalizeRotationsOfZ[rotationIndex]);
        matrix m2 = m * rotationMatrix;
        coordinate c2 = CoordinateFromMatrix(m2);
        DebugLog("Rotation %d: %d,%d,%d\n", rotationIndex, c2.X, c2.Y, c2.Z);
    }
*/
    return 0;
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

    DebugLog("- Day 19 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}