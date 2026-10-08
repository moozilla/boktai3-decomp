#include "global.h"
void sub_08112E70(u8 *);
void sub_08112E8C(u8 *p)
{
    if (p[0x1a]) {
        sub_08112E70(p);
        p[0x19] = 0;
        p[0x1a] = 0;
    }
}
