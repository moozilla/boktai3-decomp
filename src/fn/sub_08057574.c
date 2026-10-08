#include "global.h"
struct P { u8 f[0x18]; u16 f18; u8 f1a[2]; u32 f1c; };
void sub_0821AD08(u32, u32);
void sub_08057538(void);
s32 sub_08057574(struct P *p)
{
    u32 v = p->f18 - 1;
    p->f18 = v;
    if ((s32)(v << 16) == 0) {
        if (p->f1c != 0)
            sub_0821AD08(p->f1c, 0);
        sub_08057538();
    }
    return 0;
}
