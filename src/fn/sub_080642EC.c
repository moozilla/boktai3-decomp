#include "global.h"
void sub_0824923C(u8 *, u32);
void sub_080641B0(u8 *);
void sub_08225B74(u8 *);
u32 sub_080642EC(u8 *p)
{
    sub_0824923C(p, *(u32 *)(p + 0x140));
    sub_080641B0(p);
    sub_08225B74(p + 0x1c);
    return 0;
}
