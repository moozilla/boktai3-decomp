#include "global.h"
struct Elem {
    u8 pad[8];
    u32 flags;
};
void sub_081BB13C(u32);
void sub_081B984C(u8 *, void (*)(void));
void sub_081BA19C(void);
void sub_081BA12C(u8 *p, s32 count, s32 b)
{
    u32 one;
    if (count > 0)
        p[0x522] = count;
    else
        p[0x522] = 7;
    *(u16 *)(p + 0x520) = 0;
    p[0x523] = b;
    *(u16 *)(p + 0x52A) = one = 1;
    sub_081BB13C(*(u32 *)(p + 0x558));
    ((struct Elem *)(p + 0x440))->flags |= one;
    ((struct Elem *)(p + 0x4A0))->flags |= one;
    sub_081B984C(p, sub_081BA19C);
}
