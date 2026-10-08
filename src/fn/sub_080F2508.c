#include "global.h"

struct P { u8 f[0xcd4]; void (*cb)(void); u8 g[0x1c]; s32 n; };
void sub_080F2538(void);

void sub_080F2508(struct P *p)
{
    if (--p->n == 0) {
        p->n = 300;
        p->cb = sub_080F2538;
    }
}
