#include "global.h"
void sub_0822B2F8(u32);
void sub_08220F70(u8 *, u8 *);
void sub_081BC76C(u8 *);
void sub_081BCB18(u8 *p)
{
    u16 *c;
    sub_08220F70(p + 0x58, p + 0x18);
    c = (u16 *)(p + 0x904);
    if (*c == 0x20) sub_0822B2F8(0x2c0);
    (*c)++;
    if (*c > 0x68) sub_081BC76C(p);
}
