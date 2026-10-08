#include "global.h"

struct P { u8 f[0x1c]; u8 a; u8 g[7]; u8 b; };
extern struct P *gUnk_020004B4;
void sub_0822B058(s32);

static inline u8 chk(struct P *p)
{
    if (p->a == 0)
        return TRUE;
    return FALSE;
}

void sub_081092F0(u32 v)
{
    if (gUnk_020004B4 != 0) {
        struct P *p = gUnk_020004B4;
        if (!chk(p)) {
            p->b = v;
            sub_0822B058(1);
        }
    }
}
