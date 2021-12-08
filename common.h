#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#if NO_ASSERTS
#define Assert(expression)
#else
#define Assert(expression) \
	if (!(expression))     \
	{                      \
		*(int *)0 = 0;     \
	}
#endif

#define DebugLog(...) fprintf(stdout, __VA_ARGS__)
#define ErrorLog(...) fprintf(stdout, __VA_ARGS__)

#define InvalidCodePath Assert(!"InvalidCodePath")
#define InvalidDefaultCase \
	default:               \
	{                      \
		InvalidCodePath;   \
	}                      \
	break

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8 b8;
typedef int8 s8;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;

typedef bool32 b32;

typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef float real32;
typedef double real64;

typedef real32 r32;
typedef real64 r64;
typedef real32 f32;
typedef real64 f64;

typedef size_t memory_index;

struct file_data
{
    size_t Size;
    char *Data;
};

file_data
ReadToEndOfFile(char *filename)
{
    file_data result = {};

    FILE *file = fopen(filename, "rb");

    if (file)
    {
        fseek(file, 0, SEEK_END);
        result.Size = ftell(file);
        fseek(file, 0, SEEK_SET);

        result.Data = (char *)malloc(result.Size);
        fread(result.Data, result.Size, 1, file);

        fclose(file);
    }
    else
    {
        DebugLog("Could not open file %s\n", filename);
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
QS_Partition(u32 *values, s32 low, s32 high) 
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
QS_Sort(u32 *values, s32 low, s32 high)
{
    if (low < high)
    { 
        u32 partitionIndex = QS_Partition(values, low, high); 
        QS_Sort(values, low, partitionIndex - 1); 
        QS_Sort(values, partitionIndex + 1, high); 
    } 
}

#endif