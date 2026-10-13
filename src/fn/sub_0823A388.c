#include "global.h"
void sub_0823233C(u8 *);
void sub_08013684(u8 *);
void Effect_Remove(u8 *);
void sub_08239130(u8 *);
void sub_082395EC(u8 *);
void sub_08238F10(u8 *);
void sub_0823A388(u8 *p)
{
    sub_0823233C(p + 0x5b8);
    sub_08013684(p + 0x700);
    Effect_Remove(p + 0x748);
    Effect_Remove(p + 0x7d0);
    sub_08239130(p);
    sub_082395EC(p);
    sub_08238F10(p);
}
