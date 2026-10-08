#include "global.h"
void sub_08020D68(u8 *, u32);
void sub_081CCA90(u8 *a, u8 *b)
{
    if (b[8]) {
        b[8] = 0;
        b[7] = 0;
        b[5] = 0x2a;
        sub_08020D68(b + 0x168, 1);
    }
    if (b[7]) sub_08020D68(b + 0x168, 1);
}
