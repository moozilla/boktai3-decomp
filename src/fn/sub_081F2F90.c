#include "global.h"

struct RatatoskPoint {
    s16 x, y, z, pad;
};

s32 sub_081F2F90(u8 *p)
{
    s32 idx = -1;
    u32 best = 0;
    s32 i = 0;
    u8 *q = p + 0xD28;
    do {
        u32 v = *(u32 *)q;
        if (v >= best) {
            best = v;
            idx = i;
        }
        q += 0x18;
        i++;
    } while (i <= 8);
    return idx;
}

s32 sub_081F2FBC(u8 *p)
{
    s32 idx = -1;
    u32 best = 0;
    s32 i = 0;
    u8 *q = p + 0xD24;
    do {
        u32 v = *(u32 *)q;
        if (v >= best) {
            best = v;
            idx = i;
        }
        q += 0x18;
        i++;
    } while (i <= 8);
    return idx;
}

s32 sub_081F2FE8(u8 *p)
{
    s32 idx = -1;
    u32 best = 0;
    s32 i = 0;
    u8 *q = p + 0xD24;
    do {
        u32 v = ((u32 *)q)[1] + ((u32 *)q)[0];
        if (v >= best) {
            best = v;
            idx = i;
        }
        q += 0x18;
        i++;
    } while (i <= 8);
    return idx;
}

struct RatatoskPoint *sub_081F3018(u8 *p, s16 *pos)
{
    s32 idx = -1;
    u32 best = 0xFFFFFFFF;
    s32 i = 0;
    s32 px = pos[0];
    s32 pz = pos[2];
    struct RatatoskPoint *q = (struct RatatoskPoint *)(p + 0xCCC);
    do {
        s32 dx = q->x - px;
        s32 dz = q->z - pz;
        u32 d = dx * dx + dz * dz;
        if (d <= best) {
            idx = i;
            best = d;
        }
        q++;
        i++;
    } while (i <= 8);
    if (idx == -1)
        return NULL;
    return (struct RatatoskPoint *)(p + 0xCCC) + idx;
}
