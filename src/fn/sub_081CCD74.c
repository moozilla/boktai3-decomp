#include "global.h"
void sub_08020D68(u8 *, u32);
void sub_081CCD74(u8 *a, u8 *b)
{
    if (b[8]) {
        b[8] = 0;
        b[7] = 0;
        b[5] = 0x3c;
    }
    if (b[7]) {
        if (b[6] == 0x3c) b[5] = 0x3d;
        else sub_08020D68(b + 0x168, 1);
    }
}
