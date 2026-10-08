#include "global.h"
void sub_08214514(u8 *);
void sub_08225938(u8 *);
u32 sub_08053AC4(u8 *p)
{
    sub_08214514(p + 0x68);
    sub_08214514(p + 0xB8);
    if (p[0x1B] != 0)
        sub_08225938(p + 0x20);
    return 0;
}
