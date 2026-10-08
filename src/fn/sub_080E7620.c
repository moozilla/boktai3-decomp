#include "global.h"

u32 sub_080E81CC(u32);
void sub_080763D0(void *, u32, u32);

struct W { u16 pad[3]; u16 f; };

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag)
    {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080E7620(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
    {
        u32 r = sub_080E81CC(*(u8 *)(p + 0x59));
        struct W *w;
        sub_080763D0(p, 0, r);
        w = (struct W *)(p + 0x188);
        w->f = 4 | w->f;
    }
}
