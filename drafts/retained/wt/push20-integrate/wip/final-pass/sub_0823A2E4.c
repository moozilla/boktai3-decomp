#include "global.h"
void sub_0823A060(u8 *);
void sub_0823A2BC(u8 *);
s32 sub_08239F74(u8 *, s32);
s32 sub_08239EB4(u8 *, s32);
s32 sub_0823A028(u8 *, s32);
void sub_08239A20(u8 *, s32);
void sub_082386DC(u8 *);
void sub_08238814(u8 *);
void sub_08238FE0(u8 *);
void sub_082391A8(u8 *);
void sub_0824923C(u8 *, u32);
void sub_0823A2E4(u8 *p)
{
    sub_0823A060(p);
    sub_0823A2BC(p);
    sub_08239A20(p, sub_0823A028(p, sub_08239EB4(p, sub_08239F74(p, 0))));
    sub_082386DC(p);
    sub_08238814(p);
    sub_08238FE0(p);
    sub_082391A8(p);
    {
        u32 *source = (u32 *)(p + 0x864);
        u8 *target = p + 0x7fc;
        sub_0824923C(target, *source);
    }
    {
        u32 v = *(u16 *)(p + 0x6f8);
        u32 add = *(u16 *)(p + 0x30);
        *(u16 *)(p + 0x6f0) = v + add;
    }
    {
        u32 v = *(u16 *)(p + 0x6fa);
        u32 add = *(u16 *)(p + 0x32);
        *(u16 *)(p + 0x6f2) = v + add;
    }
    {
        u32 v = *(u16 *)(p + 0x6fc);
        u32 add = *(u16 *)(p + 0x34);
        *(u16 *)(p + 0x6f4) = v + add;
    }
}
