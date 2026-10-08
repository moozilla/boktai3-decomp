#include "global.h"

struct Bg {
    u8 pad0[0x10];
    u16 f10, f12;
    u8 pad14[4];
    s16 f18;
    u8 pad1A[0x12];
    u16 *map;
};
extern struct Bg gUnk_03004C30[];

void sub_08215DF0(u32 n, s32 x, s32 y, u32 a, u32 b, u32 c)
{
    struct Bg *bg = &gUnk_03004C30[n];
    bg->map[(bg->f18 << 1) * y + x] = a | (b << 10) | (c << 12);
}
