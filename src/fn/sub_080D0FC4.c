#include "global.h"

void sub_08076034(void *, s32);

struct S { u8 filler[0x2b2]; u8 f; };

void sub_080D0FC4(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
    sub_08076034(s, 0);
}
