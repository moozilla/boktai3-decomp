#include "global.h"
void sub_08220F70(u8 *, u8 *);
void sub_0801A634(u8 *p)
{
    u8 *e = p + 0x194;
    s32 i = 2;
    do {
        sub_08220F70(e, p + 0x4c);
        e += 0x60;
        i--;
    } while (i >= 0);
}
