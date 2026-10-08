#include "global.h"

struct S080F54D4 { u8 filler[0x178]; void *fn; u8 filler2[0x184 - 0x17c]; s32 cnt; };
void sub_080F5504(void);
void sub_0822B2F8(u32);

void sub_080F54D4(struct S080F54D4 *s)
{
    if (--s->cnt == 0) {
        s->fn = sub_080F5504;
        sub_0822B2F8(0x16a);
    }
}
