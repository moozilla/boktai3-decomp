#include "global.h"
extern u8 *gUnk_02000488;
struct P { u32 a, b; };
void sub_08049B3C(struct P *a, u32 b)
{
    u8 *p = gUnk_02000488;
    if (p) {
        *(struct P *)(p + 0x3F0) = *a;
        *(u16 *)(p + 0x3F6) = b;
    }
}
