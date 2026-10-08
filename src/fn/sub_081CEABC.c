#include "global.h"
struct P { u32 a, b; };
void sub_08020D68(u8 *, u32);
void sub_081CEABC(u8 *a, u8 *b)
{
    if (b[8]) {
        b[8] = 0;
        b[7] = 0;
        *(struct P *)(b + 0x54) = *(struct P *)(b + 0x68);
        b[5] = 3;
        sub_08020D68(b + 0x168, 1);
    }
    *(u32 *)(b + 0x14) += 1;
}
