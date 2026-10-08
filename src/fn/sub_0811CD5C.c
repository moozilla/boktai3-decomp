#include "global.h"
void sub_0811CD5C(s32 p, u32 v)
{
    s32 q = p + 0x180;
    do {
        *(u8 *)(q + 0x1a) = v;
        q -= 0x60;
    } while (q >= p);
}
