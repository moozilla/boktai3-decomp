#include "global.h"

struct S { u8 filler[0x6a8]; u8 b; };

void sub_080DF6D4(struct S *s)
{
    u8 *p = &s->b;
    if (*p != 0)
        *p = *p - 1;
}
