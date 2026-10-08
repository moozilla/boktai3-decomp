#include "global.h"
struct Z { u8 f[0x20]; };
struct Y { u8 f[0xc]; u8 c; };
extern u8 *gUnk_02000274;
void sub_081D1150(void)
{
    u8 *p = gUnk_02000274;
    struct Y *q;
    if (p) {
        q = (struct Y *)(p + 0x20);
        q->c = 1;
    }
}
