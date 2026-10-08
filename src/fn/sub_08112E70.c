#include "global.h"
void sub_08112E20(u8 *);
void sub_08112E48(u8 *);
void sub_08112E70(u8 *p)
{
    if (p[0x1b] == 0)
        sub_08112E20(p);
    else
        sub_08112E48(p);
}
