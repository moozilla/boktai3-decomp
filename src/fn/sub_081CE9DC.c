#include "global.h"
void sub_08020D68(u8 *, u32);
void sub_081CE9DC(u8 *a, u8 *b)
{
    if (b[8]) {
        b[8] = 0;
        b[7] = 0;
        b[5] = 0xf;
    }
    if (b[7]) {
        u32 z = 0;
        b[4] = z;
        *(u32 *)(b + 0x14) = z;
        b[8] = 1;
        sub_08020D68(b + 0x168, 1);
    }
}
