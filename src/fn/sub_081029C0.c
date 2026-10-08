#include "global.h"

struct S { u8 filler[0xb6]; u16 h; u8 filler2[0xdc - 0xb8]; u32 k; };
void sub_08049F78(u32, u32);
void sub_08049FA8(u32);

void sub_081029C0(struct S *s)
{
    if ((u16)(s->h - 3) > 1) {
        sub_08049F78(7, s->k);
    } else {
        sub_08049F78(3, s->k);
        sub_08049FA8(4);
    }
}
