#include "global.h"

void sub_08217EAC(u8 *);

void sub_082395EC(u8 *p)
{
    u8 *q = p + 0x9B0;
    s32 i;

    for (i = 3; i >= 0; i--) {
        sub_08217EAC(q);
        q += 0x34;
    }
}
