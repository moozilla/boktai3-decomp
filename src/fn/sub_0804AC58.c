#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
extern u8 *gUnk_02000710;
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void sub_0804AC58(u8 *p)
{
    u8 *q = p + 0x184;
    u8 *g = gUnk_02000710;
    s32 v = *(s16 *)(g + 0x5B0);
    *(u16 *)(p + 0x20) = v * 10 + 5 - *(u16 *)(g + 0x876);
    sub_0821FF58(q, *(u16 *)(p + 0x20), *(u16 *)(p + 0x22), 0, 0x40000, 0x1E);
}
