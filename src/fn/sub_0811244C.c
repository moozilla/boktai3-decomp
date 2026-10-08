#include "global.h"

struct S { u8 filler[0x000011CA]; u8 f; };

void sub_0811244C(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
}
