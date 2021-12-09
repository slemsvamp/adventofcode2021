#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct game_board
{
    u8 Cell[25];
    b8 Won;
};

struct game_setup
{
    u8 *Numbers;
    u32 QuantityNumbers;
    game_board *Boards;
    u32 QuantityBoards;
};

struct board_result
{
    u32 Sum;
    u8 Number;
    u32 BoardIndex;
};

internal u32
GetQuantityBoards(char *buffer, file_data file)
{
    u32 result = 0;
    char *playhead = buffer;

    u32 emptyLines = 0;
    while ((u32)(playhead + 3 - file.Data) < file.Size)
    {
        if (*playhead == '\r' && *(playhead + 1) == '\n' && *(playhead + 2) == '\r' && *(playhead + 3) == '\n')
            emptyLines++;
        playhead++;
    }

    result = emptyLines + 1;
    return result;
}

internal game_board
GetBoard(char *buffer)
{
    game_board result = {};

    for (u8 row = 0; row < 5; row++)
    {
        for (u8 column = 0; column < 5; column++)
        {
            while (*buffer == ' ')
                buffer++;
            u8 value = (u8)atoi(buffer);
            while (*buffer >= '0' && *buffer <= '9')
                buffer++;
            *(result.Cell + (row * 5) + column) = value;
        }
        
        while (*buffer < '0' || *buffer > '9')
            buffer++;
    }

    result.Won = 0;

    return result;
}

internal game_board*
GetBoards(char *buffer, u32 quantity)
{
    game_board *result = (game_board *)malloc(sizeof(game_board) * quantity);
    u32 boardSizeInFile = 16 /* columns in bytes */ * 5 /* rows */ + 2 /* \r\n */;

    for (u32 boardIndex = 0; boardIndex < quantity; boardIndex++)
        *(result + boardIndex) = GetBoard(buffer + (boardIndex * boardSizeInFile));

    return result;
}

internal game_setup
GetGameSetup(file_data file)
{
    char *playhead = file.Data;
    char *initial = playhead;

    game_setup result = {};

    u32 commas = 0;
    while (*playhead != '\r')
    {
        if (*playhead == ',')
            commas++;
        playhead++;
    }
    playhead = initial;

    result.QuantityNumbers = commas + 1;
    result.Numbers = (u8 *)malloc(sizeof(u8) * result.QuantityNumbers);

    u32 index = 0;
    while (*playhead != '\r')
    {
        if (*playhead != ',')
        {
            *(result.Numbers + index++) = (u8)atoi(playhead);
            while (*playhead >= '0' && *playhead <= '9')
                playhead++;
        }
        
        playhead++;
    }

    while (*playhead < '0' || *playhead > '9')
        playhead++;

    result.QuantityBoards = GetQuantityBoards(playhead, file);
    result.Boards = GetBoards(playhead, result.QuantityBoards);

    return result;
}

internal void
CheckNumberOnBoard(u8 number, game_board *board)
{
    for (u8 cellIndex = 0; cellIndex < 25; cellIndex++)
    {
        u8 *cell = board->Cell + cellIndex;
        if (*cell == number)
            *cell = 0;
    }
}

internal b8
HasBoardWon(game_board *board)
{
    for (u8 rowIndex = 0; rowIndex < 5; rowIndex++)
    {
        u32 sum = 0;
        for (u8 columnIndex = 0; columnIndex < 5; columnIndex++)
            sum += *(board->Cell + (rowIndex * 5) + columnIndex);
        if (sum == 0)
            return 1;
    }

    for (u8 columnIndex = 0; columnIndex < 5; columnIndex++)
    {
        u32 sum = 0;
        for (u8 rowIndex = 0; rowIndex < 5; rowIndex++)
            sum += *(board->Cell + (rowIndex * 5) + columnIndex);
        if (sum == 0)
            return 1;
    }

    return 0;
}

internal u32
Part1()
{
    file_data file = ReadToEndOfFile("input\\day04-input1.txt");

    game_setup gameSetup = GetGameSetup(file);

    for (u32 drawIndex = 0; drawIndex < gameSetup.QuantityNumbers; drawIndex++)
    {
        u8 number = *(gameSetup.Numbers + drawIndex);

        for (u32 boardIndex = 0; boardIndex < gameSetup.QuantityBoards; boardIndex++)
        {
            game_board *board = gameSetup.Boards + boardIndex;
            CheckNumberOnBoard(number, board);

            if (HasBoardWon(board))
            {
                u32 sum = 0;
                for (u32 cellIndex = 0; cellIndex < 25; cellIndex++)
                    sum += *(board->Cell + cellIndex);

                return sum * number;
            }
        }
    }
}

internal u32
Part2()
{
    file_data file = ReadToEndOfFile("input\\day04-input1.txt");

    game_setup gameSetup = GetGameSetup(file);

    board_result lastBoard = {};

    for (u32 drawIndex = 0; drawIndex < gameSetup.QuantityNumbers; drawIndex++)
    {
        u8 number = *(gameSetup.Numbers + drawIndex);

        for (u32 boardIndex = 0; boardIndex < gameSetup.QuantityBoards; boardIndex++)
        {
            game_board *board = gameSetup.Boards + boardIndex;
            CheckNumberOnBoard(number, board);

            if (!board->Won && HasBoardWon(board))
            {
                u32 sum = 0;
                for (u32 cellIndex = 0; cellIndex < 25; cellIndex++)
                    sum += *(board->Cell + cellIndex);

                lastBoard.Number = number;
                lastBoard.Sum = sum;
                lastBoard.BoardIndex = boardIndex;

                board->Won = 1;
            }
        }
    }

    return lastBoard.Sum * lastBoard.Number;
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

    DebugLog("- Day 04 -\n");
    DebugLog("Result Part 1: %d (%d ms, %lld cycles passed)\n", resultPart1, (part1Time - startTime) * 1000 / CLOCKS_PER_SEC, (part1Cycles - startCycles));
    DebugLog("Result Part 2: %d (%d ms, %lld cycles passed)\n", resultPart2, (endTime - part1Time) * 1000 / CLOCKS_PER_SEC, (endCycles - part1Cycles));
    DebugLog("\n");

    return 0;
}