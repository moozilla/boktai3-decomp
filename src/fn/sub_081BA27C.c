#include "global.h"
void sub_081BA064(void);
void sub_081B98D8(u8 *);
void sub_0824923C(u8 *, u32);
void sub_081BA27C(u8 *p)
{
    sub_081BA064();
    sub_081B98D8(p);
    if (*(u32 *)(p + 0x55C)) sub_0824923C(p, *(u32 *)(p + 0x55C));
}
