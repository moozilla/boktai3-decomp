#include "global.h"
void sub_081D5C08(u8 *, u32);
void sub_081D5CE0(u8 *);
void sub_081D6194(u8 *p, u8 v)
{
    u8 *q = p + 0x24;
    u32 z = 0;
    *q = v;
    *(u8 *)(p + 0xDA7) = z;
    sub_081D5C08(p, *q + 0xd);
    sub_081D5CE0(p);
}
