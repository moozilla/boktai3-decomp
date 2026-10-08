#include "global.h"

u8 sub_08138168(u8 *);
void sub_08177DEC(s32);

u32 sub_0810B704(u8 *p)
{
    u8 r = sub_08138168(p);
    if (r != 0) {
        sub_08177DEC(7);
        p[0x1e] = 1;
    }
}
