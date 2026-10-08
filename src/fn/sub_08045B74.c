#include "global.h"

void sub_082195E0(u8 *);
void sub_08045B74(u8 *p)
{
    s32 i;
    u8 *e = p + 0x58;
    for (i = 4; i >= 0; i--, e += 0x60)
        sub_082195E0(e);
}
