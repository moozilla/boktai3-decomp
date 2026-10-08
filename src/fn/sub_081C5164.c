#include "global.h"

struct S { u8 pad[0xd8]; s32 fd8; };
s32 sub_081C503C(u8 *, s32, s32);
void sub_081C5088(struct S *);

void sub_081C5164(struct S *p)
{
    u8 *a = (u8 *)p + 0x18;
    s32 *q = &p->fd8;
    if (sub_081C503C(a, *q, 8)) {
        sub_081C5088(p);
    } else {
        *q = *q + 1;
    }
}
