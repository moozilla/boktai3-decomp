#include "global.h"
void sub_0805DF08(u32);
void sub_0805DE3C(u32);
void sub_0824923C(u8 *, u32);
void sub_081BFEAC(u8 *p)
{
    u8 *q = p + 0x175E;
    sub_0805DF08(*q);
    sub_0805DE3C(*q);
    sub_0824923C(p, *(u32 *)(p + 0x17F0));
}
