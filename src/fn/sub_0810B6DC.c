#include "global.h"
void sub_08138168(u8 *);
void sub_08138154(u8 *, u32, void (*)(void));
void sub_0810B704(void);
u32 sub_0810B6DC(u8 *p)
{
    u32 v;
    sub_08138168(p);
    v = p[0x2B];
    if (v != 0)
        sub_08138154(p, 0x1C, sub_0810B704);
}
