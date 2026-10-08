#include "global.h"

void sub_08109AEC(u8 *);
u32 sub_08228DB8(void);

void sub_08109BD0(u8 *p)
{
    sub_08109AEC(p);
    *(u8 *)(p + 0x20C) = 0;
    *(u8 *)(p + 0x11D) = sub_08228DB8();
}
