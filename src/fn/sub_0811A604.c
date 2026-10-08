#include "global.h"
void sub_0811A030(void *);
void sub_0811A158(void *);
void sub_0824923C(void *, u32);
u32 sub_0811A604(u8 *p)
{
    sub_0811A030(p);
    sub_0824923C(p, *(u32 *)(p + 0x86C));
    sub_0811A158(p);
    return 0;
}
