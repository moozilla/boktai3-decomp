#include "global.h"
void Registry1745_ToggleConnectionRole(u8 *p)
{
    u8 *q = p + 0x4c;
    u8 v = *q;
    if (v == 0) {
        *q = 1;
        p[0x4d] = v;
    } else {
        *q = 0;
        p[0x4d] = 1;
    }
}
