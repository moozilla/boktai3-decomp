#include "global.h"

void sub_080D6FDC(void *);

struct S { u8 filler[0x2d4]; u16 f; };

void sub_080D7548(struct S *s)
{
    u32 m = 0x100;
    if (!(s->f & m))
        sub_080D6FDC(s);
}
