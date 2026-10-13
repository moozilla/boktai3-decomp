#include "global.h"
typedef struct {
    u8 id, pad;
    u16 level;
    u8 data[12];
} Entry;
typedef struct {
    unsigned count : 16;
    int *values;
} Args;
Entry *sub_0822D6C8(int);
void Script_Call(int, Args *);
static inline int clamp(int x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}
void Transformation_ReportResult(u8 *p)
{
    int values[7];
    Args args;
    Entry *entry = sub_0822D6C8(p[0xba]);
    values[0] = clamp(*(int *)(p + 0xd8));
    values[1] = clamp(*(int *)(p + 0xdc));
    values[2] = clamp(*(int *)(p + 0xe0));
    values[3] = *(s16 *)(p + 0xc8);
    values[4] = p[0xc6];
    values[5] = p[0xc5];
    values[6] = entry->level;
    args.count = 7;
    args.values = values;
    Script_Call(*(u16 *)(p + 0x7a0), &args);
}
