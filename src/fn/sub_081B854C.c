#include "global.h"
void sub_08214514(u8 *);
void sub_0821FE6C(u8 *);
void sub_08013684(u8 *);
void sub_081B854C(u8 *p)
{
    sub_08214514(p + 0x18);
    sub_0821FE6C(p + 0x94);
    if (*(s16 *)(p + 0xec) == 0xc) sub_08013684(p + 0xfc);
}
