#include "global.h"

void sub_0823A2BC(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x348);

    if (*(u16 *)(q + 2) & 0xF0) {
        p[0x4F4]++;
    }
}
