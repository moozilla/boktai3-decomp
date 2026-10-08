#include "global.h"

void sub_08214514(u8 *);

void sub_0812B610(u8 *p)
{
    u8 *q = p + 0x3C;
    s32 i;

    for (i = 1; i >= 0; i--) {
        sub_08214514(q);
        q += 0x5C;
    }
}
