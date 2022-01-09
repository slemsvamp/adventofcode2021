#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <time.h>
#include <windows.h>

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

union matrix
{
    struct
    {
        s32 M00;
        s32 M01;
        s32 M02;
        s32 M03;
        s32 M10;
        s32 M11;
        s32 M12;
        s32 M13;
        s32 M20;
        s32 M21;
        s32 M22;
        s32 M23;
        s32 M30;
        s32 M31;
        s32 M32;
        s32 M33;
    };
    s32 E[16];
};

inline matrix
MatrixIdentity()
{
    matrix m = {};
    m.M00 = 1;
    m.M01 = 0;
    m.M02 = 0;
    m.M03 = 0;
    m.M10 = 0;
    m.M11 = 1;
    m.M12 = 0;
    m.M13 = 0;
    m.M20 = 0;
    m.M21 = 0;
    m.M22 = 1;
    m.M23 = 0;
    m.M30 = 0;
    m.M31 = 0;
    m.M32 = 0;
    m.M33 = 1;
    return m;
}

inline matrix
MatrixTranslation(s32 x, s32 y, s32 z)
{
    matrix m;
    m.M00 = 1;
    m.M01 = 0;
    m.M02 = 0;
    m.M03 = 0;
    m.M10 = 0;
    m.M11 = 1;
    m.M12 = 0;
    m.M13 = 0;
    m.M20 = 0;
    m.M21 = 0;
    m.M22 = 1;
    m.M23 = 0;
    m.M30 = x;
    m.M31 = y;
    m.M32 = z;
    m.M33 = 1;
    return m;
}

inline matrix
MatrixRotationX()
{
    matrix m = {};

    m.M00 = 1; m.M01 = 0; m.M02 = 0;  m.M03 = 0;
    m.M10 = 0; m.M11 = 0; m.M12 = -1; m.M13 = 0;
    m.M20 = 0; m.M21 = 1; m.M22 = 0;  m.M23 = 0;
    m.M30 = 0; m.M31 = 0; m.M32 = 0;  m.M33 = 1;

    return m;
}

inline matrix
MatrixRotationY()
{
    matrix m = {};

    m.M00 = 0;  m.M01 = 0; m.M02 = 1; m.M03 = 0;
    m.M10 = 0;  m.M11 = 1; m.M12 = 0; m.M13 = 0;
    m.M20 = -1; m.M21 = 0; m.M22 = 0; m.M23 = 0;
    m.M30 = 0;  m.M31 = 0; m.M32 = 0; m.M33 = 1;

    return m;
}

inline matrix
MatrixRotationZ()
{
    matrix m = {};

    m.M00 = 0; m.M01 = -1; m.M02 = 0; m.M03 = 0;
    m.M10 = 1; m.M11 = 0;  m.M12 = 0; m.M13 = 0;
    m.M20 = 0; m.M21 = 0;  m.M22 = 1; m.M23 = 0;
    m.M30 = 0; m.M31 = 0;  m.M32 = 0; m.M33 = 1;

    return m;
}

inline matrix
operator*(matrix left, matrix right)
{
    matrix result;

    result.M00 = (left.M00 * right.M00) + (left.M01 * right.M10) + (left.M02 * right.M20) + (left.M03 * right.M30);
    result.M01 = (left.M00 * right.M01) + (left.M01 * right.M11) + (left.M02 * right.M21) + (left.M03 * right.M31);
    result.M02 = (left.M00 * right.M02) + (left.M01 * right.M12) + (left.M02 * right.M22) + (left.M03 * right.M32);
    result.M03 = (left.M00 * right.M03) + (left.M01 * right.M13) + (left.M02 * right.M23) + (left.M03 * right.M33);

    result.M10 = (left.M10 * right.M00) + (left.M11 * right.M10) + (left.M12 * right.M20) + (left.M13 * right.M30);
    result.M11 = (left.M10 * right.M01) + (left.M11 * right.M11) + (left.M12 * right.M21) + (left.M13 * right.M31);
    result.M12 = (left.M10 * right.M02) + (left.M11 * right.M12) + (left.M12 * right.M22) + (left.M13 * right.M32);
    result.M13 = (left.M10 * right.M03) + (left.M11 * right.M13) + (left.M12 * right.M23) + (left.M13 * right.M33);

    result.M20 = (left.M20 * right.M00) + (left.M21 * right.M10) + (left.M22 * right.M20) + (left.M23 * right.M30);
    result.M21 = (left.M20 * right.M01) + (left.M21 * right.M11) + (left.M22 * right.M21) + (left.M23 * right.M31);
    result.M22 = (left.M20 * right.M02) + (left.M21 * right.M12) + (left.M22 * right.M22) + (left.M23 * right.M32);
    result.M23 = (left.M20 * right.M03) + (left.M21 * right.M13) + (left.M22 * right.M23) + (left.M23 * right.M33);

    result.M30 = (left.M30 * right.M00) + (left.M31 * right.M10) + (left.M32 * right.M20) + (left.M33 * right.M30);
    result.M31 = (left.M30 * right.M01) + (left.M31 * right.M11) + (left.M32 * right.M21) + (left.M33 * right.M31);
    result.M32 = (left.M30 * right.M02) + (left.M31 * right.M12) + (left.M32 * right.M22) + (left.M33 * right.M32);
    result.M33 = (left.M30 * right.M03) + (left.M31 * right.M13) + (left.M32 * right.M23) + (left.M33 * right.M33);

    return result;
}

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

        result.Data = (char *)calloc(result.Size + 1, 1);
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
Swap(s32 *one, s32 *other)
{
    s32 temp = *one;
    *one = *other;
    *other = temp;
}

internal void
Swap(u32 *one, u32 *other)
{
    u32 temp = *one;
    *one = *other;
    *other = temp;
}

internal void
Swap(u64 *one, u64 *other)
{
    u64 temp = *one;
    *one = *other;
    *other = temp;
}

inline b8
Contains(s32 *values, u32 count, s32 value)
{
    for (u32 index = 0; index < count; index++)
        if (*(values + index) == value)
            return true;
    return false;
}

inline b8
Contains(u32 *values, u32 count, u32 value)
{
    for (u32 index = 0; index < count; index++)
        if (*(values + index) == value)
            return true;
    return false;
}

inline b8
Contains(u64 *values, u32 count, u64 value)
{
    for (u32 index = 0; index < count; index++)
        if (*(values + index) == value)
            return true;
    return false;
}

internal s32
QS_Partition(s32 *values, s32 low, s32 high) 
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

internal u64
QS_Partition(u64 *values, s32 low, s32 high) 
{ 
    u64 pivot = *(values + high);
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
QS_Sort(s32 *values, s32 low, s32 high)
{
    if (low < high)
    { 
        u32 partitionIndex = QS_Partition(values, low, high); 
        QS_Sort(values, low, partitionIndex - 1); 
        QS_Sort(values, partitionIndex + 1, high); 
    } 
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

internal void
QS_Sort(u64 *values, s32 low, s32 high)
{
    if (low < high)
    { 
        u32 partitionIndex = QS_Partition(values, low, high); 
        QS_Sort(values, low, partitionIndex - 1); 
        QS_Sort(values, partitionIndex + 1, high); 
    } 
}

struct dictionary_node
{
    char *Key;
    void *Data;
    dictionary_node *Next;
};

struct dictionary
{
    u32 Slots;
    dictionary_node **Nodes;
    u32 Adds;
};

internal void
DICT_Init(dictionary *dict, u32 slots)
{
    dict->Slots = slots;
    dict->Nodes = (dictionary_node**)calloc(slots, sizeof(dictionary_node*));
}

internal u32
DICT_Hash(char *key)
{
    // djb2
    u32 hash = 5381;
    u32 c;

    while (c = *key++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

internal void
DICT_Add(dictionary *dict, char *key, void *data)
{
    u32 nodeIndex = DICT_Hash(key) % dict->Slots;

    dictionary_node *node = *(dict->Nodes + nodeIndex);
    dictionary_node *newNode = (dictionary_node*)malloc(sizeof(dictionary_node));
    newNode->Key = key;
    newNode->Data = data;
    newNode->Next = 0;

    if (node == NULL)
        *(dict->Nodes + nodeIndex) = newNode;
    else
    {
        while (node->Next)
            node = node->Next;
        node->Next = newNode;
    }

    dict->Adds++;
}

internal dictionary_node*
DICT_Get(dictionary *dict, char *key)
{
    u32 nodeIndex = DICT_Hash(key) % dict->Slots;
    dictionary_node *node = *(dict->Nodes + nodeIndex);

    if (node == NULL)
        return 0;

    while (node)
    {
        if (strcmp(key, node->Key) == 0)
            return node;
        node = node->Next;
    }
    return 0;
}

internal b8
DICT_ContainsKey(dictionary *dict, char *key)
{
    return DICT_Get(dict, key) != NULL;
}
#endif