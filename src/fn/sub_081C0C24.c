#include "global.h"
struct E { u8 pad[0x60]; };
struct S { u8 f[0x90]; struct E e[8]; u8 g[0x454 - 0x90 - 0x300]; u8 h[0x400 - 0x394 + 0x394 - 0x400]; };
void sub_082195E0(void *);
void sub_081C0C24(u8 *p)
{
    s32 i;
    u8 *e = p + 0x90;
    for (i = 7; i >= 0; i--) { sub_082195E0(e); e += 0x60; }
    sub_082195E0(p + 0x390);
    sub_082195E0(p + 0x3f0);
}
