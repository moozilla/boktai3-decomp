#include "global.h"
void sub_082195E0(void *);
void sub_0811CD08(u8 *p)
{
    s32 i = 4;
    do {
        sub_082195E0(p);
        p += 0x60;
    } while (--i >= 0);
}
