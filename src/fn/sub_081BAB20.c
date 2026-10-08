#include "global.h"
void sub_081BA9D0(void);
void sub_081BA6F8(u8 *);
void sub_0824923C(u8 *, u32);
void sub_081BAB20(u8 *p)
{
    sub_081BA9D0();
    sub_081BA6F8(p);
    if (*(u32 *)(p + 0x260)) sub_0824923C(p, *(u32 *)(p + 0x260));
}
