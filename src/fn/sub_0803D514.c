#include "global.h"

u32 Script_GetKeywordValue(u32, u32);
u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
void sub_0803CEE8(u8 *, u8 *);
void sub_082223CC(u8 *, u32, void *);
void sub_0803C90C();

extern u32 gUnk_02000108;

s32 sub_0803D514(u8 *p)
{
    u32 n;
    u8 *m;
    u8 *e;
    s32 i;
    gUnk_02000108 = (u32)p;
    *(u32 *)(p + 0x2344) = Script_GetKeywordValue(0x61, 1);
    *(u32 *)(p + 0x1c) = 0;
    n = Script_GetKeywordValue(0x6d, 0x60);
    *(u32 *)(p + 0x18) = n;
    m = Heap_Alloc(n * 0x17c);
    *(u8 **)(p + 0x20) = m;
    if (m == 0)
        return -1;
    Memory_Clear(m, *(u32 *)(p + 0x18) * 0x17c);
    e = p + 0x44;
    i = 3;
    do {
        sub_0803CEE8(p, e);
        e += 0x8b0;
        i--;
    } while (i >= 0);
    sub_082223CC(p + 0x2354, 0x10, sub_0803C90C);
    return 0;
}
