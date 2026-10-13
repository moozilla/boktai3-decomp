#include "global.h"
void Actor_PlayAnimationEntry(u8 *, u32, u32, u32, u32);
void sub_0824923C(u8 *, u32);
void sub_081CEE18(u8 *a, u8 *b)
{
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        Actor_PlayAnimationEntry(b, 8, 1, 0, 4);
    }
    sub_0824923C(b, *(u32 *)(b + 0x2e4));
}
