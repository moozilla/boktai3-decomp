#include "global.h"

struct S { u8 filler[0x000002B1]; u8 f; };

void sub_080EB8B4(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
}
