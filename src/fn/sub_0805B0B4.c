#include "global.h"
struct P { u8 f[0x6a0]; u16 f6a0, f6a2, f6a4; u16 f6a6; u16 f6a8, f6aa, f6ac; };
void sub_0805B0B4(struct P *p)
{
    p->f6a0 = 0xa0;
    p->f6a2 = 0xa0;
    p->f6a4 = 0xa0;
    p->f6a8 = 0x40;
    p->f6aa = 0x20;
    p->f6ac = 0x40;
}
