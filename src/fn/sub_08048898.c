#include "global.h"

void sub_08217EAC(u8 *);
void sub_08048898(u8 *p)
{
    s32 i;
    u8 *e = p + 0x264;
    for (i = 6; i >= 0; i--, e += 0x28)
        sub_08217EAC(e);
}
