#include "global.h"
void sub_08138168(u8 *);
void sub_08138154(u8 *, u32, void (*)(void));
void sub_0810B63C(void);
u32 sub_0810B614(u8 *p)
{
    u32 v;
    sub_08138168(p);
    v = p[0x2a];
    if (v != 0)
        sub_08138154(p, 0x1a, sub_0810B63C);
}
